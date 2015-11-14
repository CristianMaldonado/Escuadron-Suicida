#include <sys/types.h>
#include <sys/socket.h>
#include "libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include "estructuras.h"
#include <string.h>
#include "paquetes.h"
#include "configuracion.h"
#include <stdbool.h>
#include <unistd.h>
#include "funciones_memoria.h"
#include <signal.h>
#include <pthread.h>
#include "log_memoria.h"
#include <ctype.h>

t_list * tlb;
t_list * lista_tabla_de_paginas;
tconfig_memoria * config;
t_log * logMem;
char * memoria;
int socketClienteSWAP;
pthread_mutex_t mutex;

void sig_handler(int numSignal){

	switch(numSignal){
		//borrar tlb
		case SIGUSR1:
			pthread_mutex_lock(&mutex);
				log_seniales(logMem, "SIGUSR1, limpiar tlb");
				limpiar_la_tlb(&tlb);
			pthread_mutex_unlock(&mutex);
		break;
		//borrar memoria
		case SIGUSR2:
			pthread_mutex_unlock(&mutex);
				log_seniales(logMem, "SIGUSR2, limpiar memoria y tlb");
				limpiar_la_tlb(&tlb);
				limpiar_memoria(&lista_tabla_de_paginas,memoria,config->tamanio_marco, socketClienteSWAP);
			pthread_mutex_unlock(&mutex);
		break;
		//volcar, en la consola es SIGIO (kill -l SIGIO <pid>)
		case SIGPOLL:
			pthread_mutex_lock(&mutex);
				log_seniales(logMem, "SIGPOLL, volcar memoria");
				volcar_memoria(memoria, config, logMem);
			pthread_mutex_unlock(&mutex);
		break;
	}
}

int main(void) {
	system("clear");
	config = leerConfiguracion();
	logMem = log_create("log.txt", "memoria.c", false, LOG_LEVEL_INFO);

	pthread_mutex_init(&mutex, NULL);

	//Definimos datos Cliente listener
	int socketServidorCPU, socketClienteCPU;

	client_init(&socketClienteSWAP, config->ipSwap, config->puertoSwap);
	//Definimos datos Server
	server_init(&socketServidorCPU, config->puertoEscucha);
	printf("Memoria lista...\n");
	server_acept(socketServidorCPU, &socketClienteCPU);
	printf("CPU aceptado...\n");

	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	signal(SIGPOLL, sig_handler);
	//creamos la representacion memoria///////////////////////////////////////////////////////////////////////////////////////
	memoria = crear_memoria(config->cantidad_marcos, config->tamanio_marco);
	lista_tabla_de_paginas = list_create();
	// creamos la tlb cache_13, si es que en el archivo de configuracion este esta activado///////////////////////////////////
	if(config->habilitadaTLB)
		tlb = inicializar_tlb(config->entradasTLB);

	tprotocolo_desde_cpu_y_hacia_swap paquete_desde_cpu;

	while(recibir_paquete_desde_cpu(&socketClienteCPU, &paquete_desde_cpu)) {
		sleep(config->retardoMemoria);
		printf("pid-> %d operacion %c\n", paquete_desde_cpu.pid, toupper(paquete_desde_cpu.cod_op));

		//system("clear");
		/*
		if (list_size(lista_tabla_de_paginas) > 0){
			int i;
			tabla_paginas * tabla = list_get(lista_tabla_de_paginas,0);
			printf("pagina:%d, op:%c\n",paquete_desde_cpu.paginas, paquete_desde_cpu.cod_op);
			printf("puntero pos: %d\n",tabla->pos_puntero);


			for (i = list_size(tabla->list_pagina_direccion) - 1; i>=0 ;i--){

				pagina_direccion * pagina = list_get(tabla->list_pagina_direccion,i);
				if (pagina->nro_pagina != -1)
					printf("%d, uso: %d, mod: %d\n", pagina->nro_pagina,pagina->en_uso,pagina->fue_modificado);
			}

			for (i = 0; i <list_size(tabla->list_pagina_direccion) ;i++){

				pagina_direccion * pagina = list_get(tabla->list_pagina_direccion,i);
				if (pagina->nro_pagina != -1)
					printf("%d, uso: %d, mod: %d\n", pagina->nro_pagina,pagina->en_uso,pagina->fue_modificado);
			}
		}
		*/

		switch (paquete_desde_cpu.cod_op) {
			case 'i': {
				pthread_mutex_lock(&mutex);
				void* buffer = serializar_a_swap(&paquete_desde_cpu);
				send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
				free(buffer);

				log_inicializar(logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas);
				tprotocolo_swap_memoria swap_memoria;
				recibir_paquete_desde_swap(socketClienteSWAP, &swap_memoria);

				if(swap_memoria.codAux != 'a')
					list_add(lista_tabla_de_paginas, inicializar_tabla_de_paginas(config->maximoMarcosPorProceso, paquete_desde_cpu.pid));

				avisar_a_cpu(paquete_desde_cpu.cod_op, swap_memoria.codAux, swap_memoria.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje, socketClienteCPU);
				free(paquete_desde_cpu.mensaje);
				free(swap_memoria.mensaje);
				pthread_mutex_unlock(&mutex);
			}
			break;

			case 'f': {
				pthread_mutex_lock(&mutex);
				void* buffer = serializar_a_swap(&paquete_desde_cpu);
				send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
				free(buffer);

				eliminar_tabla_de_proceso(paquete_desde_cpu.pid, &lista_tabla_de_paginas);
				if(config->habilitadaTLB)
					borrame_las_entradas_del_proceso(paquete_desde_cpu.pid, &tlb);

				log_info(logMem, "proceso finalizado -> pid: %d\n", paquete_desde_cpu.pid);

				avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, paquete_desde_cpu.mensaje, socketClienteCPU);
				pthread_mutex_unlock(&mutex);
			}
			break;

			case 'e':
			case 'l': {
				pthread_mutex_lock(&mutex);
				//si la tlb esta activada
				int marco_en_tlb = -1;
				if(config->habilitadaTLB)
					marco_en_tlb = dame_el_marco_de_la_pagina_en_la_tlb(&tlb, paquete_desde_cpu.pid, paquete_desde_cpu.paginas);

				if(marco_en_tlb == -1) {//si la pagina no esta en la tlb
					tabla_paginas *tabla_de_paginas = dame_la_tabla_de_paginas(paquete_desde_cpu.pid, &lista_tabla_de_paginas);
					int nro_marco = obtener_marco_pagina(tabla_de_paginas, paquete_desde_cpu.paginas,config->algoritmo_reemplazo == 'C');

					if(nro_marco != -1) {//si la pagina esta en memoria
						int nro_tlb = -1;
						char fifo = 'n'; //n = no esta habilitada la tlb
						if(config->habilitadaTLB) {
							fifo = actualizame_la_tlb(&tlb, paquete_desde_cpu.pid, nro_marco, paquete_desde_cpu.paginas);
							nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, nro_marco);
						}

						log_acceso_memoria(logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_marco);

						if(config->algoritmo_reemplazo == 'L')
							aplicar_LRU(&(tabla_de_paginas->list_pagina_direccion), paquete_desde_cpu.paginas);

						if(paquete_desde_cpu.cod_op == 'l') {
							char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");
							char * mensaje = dame_mensaje_de_memoria(&memoria, nro_marco, config->tamanio_marco);
							log_lectura_escritura('l', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, mensaje);
							avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
							free(paquete_desde_cpu.mensaje);
							free(mensaje);
						}
						else{
							char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");
							log_lectura_escritura('e', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, paquete_desde_cpu.mensaje);
							memcpy(memoria + nro_marco * config->tamanio_marco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
							poneme_en_modificado_la_entrada(tabla_de_paginas, paquete_desde_cpu.paginas);
							avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
							free(paquete_desde_cpu.mensaje);
						}
					}
					else {//si no esta en la memoria
						log_acceso_a_swap(logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas);
						if(estan_los_frames_ocupados(tabla_de_paginas->list_pagina_direccion, config->algoritmo_reemplazo == 'C')) {
							if (config->algoritmo_reemplazo != 'C'){
								pagina_direccion *pagina_ocupada = list_remove(tabla_de_paginas->list_pagina_direccion, 0);
								pagina_direccion *pagina_nueva = malloc(sizeof(pagina_direccion));

								pagina_nueva->en_uso = true;
								pagina_nueva->fue_modificado = false;
								pagina_nueva->nro_pagina = paquete_desde_cpu.paginas;
								pagina_nueva->nro_marco = pagina_ocupada->nro_marco;

								char fifo = 'n';
								list_add(tabla_de_paginas->list_pagina_direccion, pagina_nueva);
								if(config->habilitadaTLB)
									fifo = actualizame_la_tlb(&tlb, paquete_desde_cpu.pid, pagina_nueva->nro_marco, paquete_desde_cpu.paginas);

								//la pagina ocupada la paso a la swap si esta modificada
								llevar_a_swap(socketClienteSWAP,memoria,pagina_ocupada,config->tamanio_marco,paquete_desde_cpu.pid);

								//traerse la pagina nueva desde swap
								traer_de_swap(socketClienteSWAP,memoria,pagina_nueva->nro_marco,pagina_nueva->nro_pagina,config->tamanio_marco,paquete_desde_cpu.pid);

								char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");
								int nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, pagina_nueva->nro_marco);
								int nro_marco = obtener_marco_pagina(tabla_de_paginas, paquete_desde_cpu.paginas,config->algoritmo_reemplazo == 'C');

								//avisar a la cpu
								if(paquete_desde_cpu.cod_op == 'l') {
									char * mensaje = dame_mensaje_de_memoria(&memoria, nro_marco, config->tamanio_marco);
									log_lectura_escritura('l', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, mensaje);
									avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
									free(mensaje);
								}
								else{
									log_lectura_escritura('e', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, paquete_desde_cpu.mensaje);
									memcpy(memoria + nro_marco * config->tamanio_marco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
									pagina_nueva->fue_modificado = true;
									avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
								}
								free(paquete_desde_cpu.mensaje);
								free(pagina_ocupada);
							}
							/*aplicamos clock*/
							else
							{
								int pagina_reemplazada;
								bool llevarSwap = aplicar_clock_modificado(paquete_desde_cpu.paginas,&pagina_reemplazada,tabla_de_paginas->list_pagina_direccion,&(tabla_de_paginas->pos_puntero),paquete_desde_cpu.cod_op == 'e');

								int nro_marco = obtener_marco_pagina(tabla_de_paginas, paquete_desde_cpu.paginas,config->algoritmo_reemplazo == 'C');

								char fifo = 'n';
								if(config->habilitadaTLB)
									fifo = actualizame_la_tlb(&tlb, paquete_desde_cpu.pid, nro_marco, paquete_desde_cpu.paginas);

								//traerse la pagina nueva desde swap
								traer_de_swap(socketClienteSWAP,memoria,nro_marco,paquete_desde_cpu.paginas,config->tamanio_marco,paquete_desde_cpu.pid);

								if (llevarSwap){
									//la pagina ocupada la paso a la swap si esta modificada
									pagina_direccion * pagina_ocupada = malloc(sizeof(pagina_direccion));
									pagina_ocupada->nro_pagina = pagina_reemplazada;
									pagina_ocupada->nro_marco = nro_marco;

									llevar_a_swap(socketClienteSWAP,memoria,pagina_ocupada,config->tamanio_marco,paquete_desde_cpu.pid);
								}

								char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");
								int nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, nro_marco);

								//avisar a la cpu
								if(paquete_desde_cpu.cod_op == 'l') {
									char * mensaje = dame_mensaje_de_memoria(&memoria, nro_marco, config->tamanio_marco);
									log_lectura_escritura('l', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, mensaje);
									avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
									free(mensaje);
								} else {
									log_lectura_escritura('e', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, paquete_desde_cpu.mensaje);
									memcpy(memoria + nro_marco * config->tamanio_marco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
									poneme_en_modificado_la_entrada(tabla_de_paginas, paquete_desde_cpu.paginas);
									avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
								}
								free(paquete_desde_cpu.mensaje);
							}
						}
						else {//si hay algun frame libre
							int nro_marco = dame_un_marco_libre(lista_tabla_de_paginas, config->cantidad_marcos);
							if(nro_marco != -1) {
								if (config->algoritmo_reemplazo != 'C'){
									/*asignar un marco libre, bucar en las tablas de paginas de cada proceso y si hay uno libre es porque
									no figura en ninguna tabla de pagina de los proceso*/
									int i;
									for(i = 0; i < list_size(tabla_de_paginas->list_pagina_direccion) ; i++) {
										pagina_direccion * pagina = list_get(tabla_de_paginas->list_pagina_direccion, i);
										if(!pagina->en_uso) {
											pagina->en_uso = true;
											pagina->fue_modificado = false;
											pagina->nro_pagina = paquete_desde_cpu.paginas;
											pagina->nro_marco = nro_marco;

											//aplicamos fifo y lru (al final estan las mas recientemente usadas)
											list_remove(tabla_de_paginas->list_pagina_direccion, i);
											list_add(tabla_de_paginas->list_pagina_direccion, pagina);

											char fifo = 'n';
											if(config->habilitadaTLB)
												fifo = actualizame_la_tlb(&tlb, paquete_desde_cpu.pid, pagina->nro_marco, paquete_desde_cpu.paginas);

											//traerse la pagina nueva desde swap
											traer_de_swap(socketClienteSWAP,memoria,nro_marco,paquete_desde_cpu.paginas,config->tamanio_marco,paquete_desde_cpu.pid);

											char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");
											int nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, pagina->nro_marco);

											//avisar a la cpu
											if(paquete_desde_cpu.cod_op == 'l') {
												char * mensaje = dame_mensaje_de_memoria(&memoria, nro_marco, config->tamanio_marco);
												log_lectura_escritura('l', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, mensaje);
												avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
												free(mensaje);
											}
											else{
												log_lectura_escritura('e', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, paquete_desde_cpu.mensaje);
												memcpy(memoria + nro_marco * config->tamanio_marco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
												pagina->fue_modificado = true;
												avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
											}
											free(paquete_desde_cpu.mensaje);
											//cuando la tabla ya esta libre corto con el break el for
											break;
										}
									}
								}
								/*aplico clock modificado*/
								else{
									pagina_direccion * pagina = list_get(tabla_de_paginas->list_pagina_direccion,tabla_de_paginas->pos_puntero);
									pagina->en_uso = true;
									pagina->nro_pagina = paquete_desde_cpu.paginas;
									pagina->nro_marco = nro_marco;
									pagina->fue_modificado = false;

									if (tabla_de_paginas->pos_puntero + 1 < list_size(tabla_de_paginas->list_pagina_direccion))
										tabla_de_paginas->pos_puntero++;
									else
										tabla_de_paginas->pos_puntero = 0;

									char fifo = 'n';
									if(config->habilitadaTLB)
										fifo = actualizame_la_tlb(&tlb, paquete_desde_cpu.pid, pagina->nro_marco, paquete_desde_cpu.paginas);

									//traerse la pagina nueva desde swap
									traer_de_swap(socketClienteSWAP,memoria,nro_marco,paquete_desde_cpu.paginas,config->tamanio_marco,paquete_desde_cpu.pid);

									char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");
									int nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, pagina->nro_marco);

									//avisar a la cpu
									if(paquete_desde_cpu.cod_op == 'l') {
										char * mensaje = dame_mensaje_de_memoria(&memoria, nro_marco, config->tamanio_marco);
										log_lectura_escritura('l', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, mensaje);
										avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
										free(mensaje);
									}else{
										log_lectura_escritura('e', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco, paquete_desde_cpu.mensaje);
										memcpy(memoria + nro_marco * config->tamanio_marco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
										pagina->fue_modificado = true;
										avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
									}
									free(paquete_desde_cpu.mensaje);
								}
							} else {
								//avisale a la cpu que no hay mas memoria y no se puede agregar una nueva entrada de paginas
								avisar_a_cpu(paquete_desde_cpu.cod_op, 'a', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "fallo", socketClienteCPU);
								free(paquete_desde_cpu.mensaje);
							}
						}
					}
				} else {//si esta en la tlb

					int nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, marco_en_tlb);

					tabla_paginas *tabla_de_paginas = dame_la_tabla_de_paginas(paquete_desde_cpu.pid, &lista_tabla_de_paginas);
					if(config->algoritmo_reemplazo == 'L')
						aplicar_LRU(&(tabla_de_paginas->list_pagina_direccion), paquete_desde_cpu.paginas);

					log_acceso_memoria(logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, marco_en_tlb);

					if(paquete_desde_cpu.cod_op == 'l') {
						char * mensaje = dame_mensaje_de_memoria(&memoria, marco_en_tlb, config->tamanio_marco);
						log_lectura_escritura('l', "-", logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, true,marco_en_tlb, mensaje);
						avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
						free(mensaje);
					}
					else{
						memcpy(memoria + marco_en_tlb * config->tamanio_marco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
						tabla_paginas * tabla_de_paginas = dame_la_tabla_de_paginas(paquete_desde_cpu.pid, &lista_tabla_de_paginas);
						log_lectura_escritura('e', "-", logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, true, marco_en_tlb, paquete_desde_cpu.mensaje);
						poneme_en_modificado_la_entrada(tabla_de_paginas, paquete_desde_cpu.paginas);
						avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
					}
					free(paquete_desde_cpu.mensaje);
				}
			}
			pthread_mutex_unlock(&mutex);
			break;
		}
	}

	close(socketClienteSWAP);
	close(socketClienteCPU);
	close(socketServidorCPU);
	pthread_mutex_destroy(&mutex);
	return 0;
	// la lista esta alreves, list_add te agrega al final, y en fifo lo que sacamos esta al principio, lru mandamos al final
}
