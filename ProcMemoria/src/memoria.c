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

t_list * tlb;
t_list * lista_tabla_de_paginas;
tconfig_memoria * config;
t_log * logMem;
char * memoria;
int socketClienteSWAP;

pthread_mutex_t mutex;


void sigHandler(int numSignal){

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

	printf("Conectando al SWAP (%s : %s)... ", config->ipSwap, config->puertoSwap);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoSwap);
	printf("OK\n");

	//Definimos datos Server
	server_init(&socketServidorCPU, config->puertoEscucha);
	printf("Memoria lista...\n");

	server_acept(socketServidorCPU, &socketClienteCPU);
	printf("CPU aceptado...\n");

	signal(SIGUSR1, sigHandler);
	signal(SIGUSR2, sigHandler);
	signal(SIGPOLL, sigHandler);

	///////////////////////////////////////////////////////////////////////////////////////
	// creamos la representacion memoria

	memoria = crear_memoria(config->cantidad_marcos, config->tamanio_marco);
	lista_tabla_de_paginas = list_create();

	///////////////////////////////////////////////////////////////////////////////////////
	// creamos la tlb cache_13, si es que en el archivo de configuracion este esta activado
	if(config->habilitadaTLB)
		tlb = inicializar_tlb(config->entradasTLB);

	///////////////////////////////////////////////////////////////////////////////////////

	tprotocolo_desde_cpu_y_hacia_swap paquete_desde_cpu;
	int salir = 0;
	while(!salir){

		if(recibir_paquete_desde_cpu(&socketClienteCPU, &paquete_desde_cpu))
		// para probar si recibe
			printf("%s\n", paquete_desde_cpu.mensaje);
		else{
			// si no recibe termina el swap
			salir = 1;
			continue;
		}

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

				log_info(logMem, "ando");

				tprotocolo_memoria_cpu paquete_memoria_cpu;
				armar_estructura_protocolo_a_cpu(&paquete_memoria_cpu, paquete_desde_cpu.cod_op, 'f', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, paquete_desde_cpu.mensaje);
				buffer = serializar_a_cpu(&paquete_memoria_cpu);
				send(socketClienteCPU, buffer, strlen(paquete_desde_cpu.mensaje) + 15, 0);

				free(buffer);
				pthread_mutex_unlock(&mutex);
			}
			break;

			case 'e':
			case 'l': {

				pthread_mutex_lock(&mutex);
				sleep(config->retardoMemoria);
				int direccion_posta = -1;
				if(config->habilitadaTLB)
					direccion_posta = dame_la_direccion_posta_de_la_pagina_en_la_tlb(&tlb, paquete_desde_cpu.pid, paquete_desde_cpu.paginas);

				if(direccion_posta == -1) { // si la pagina no esta en la tlb

					tabla_paginas *tabla_de_paginas = dame_la_tabla_de_paginas(paquete_desde_cpu.pid, &lista_tabla_de_paginas);

					// paginas es numero de pagina o cantidad de paginas depende el protocolo, en este caso es numero de pagina
					int nro_marco = dame_la_direccion_de_la_pagina(tabla_de_paginas, paquete_desde_cpu.paginas);

					// es valida o no la direccion
					if(nro_marco != -1) { // si la pagina esta en memoria

						int nro_tlb = -1;
						char fifo = 'n'; // n = no esta habilitada la tlb
						if(config->habilitadaTLB) {
							fifo = actualizame_la_tlb(&tlb, paquete_desde_cpu.pid, nro_marco * config->tamanio_marco, paquete_desde_cpu.paginas);
							nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, direccion_posta);
						}

						log_acceso_memoria(logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_marco);

						if(paquete_desde_cpu.cod_op == 'l') {
							char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");

							log_lectura_escritura('l', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco);

							char * mensaje = dame_mensaje_de_memoria(&memoria, nro_marco, config->tamanio_marco);
							avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
							free(paquete_desde_cpu.mensaje);
							free(mensaje);
						} else {
							char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");

							log_lectura_escritura('e', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco);

							memcpy(memoria + nro_marco * config->tamanio_marco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
							poneme_en_modificado_la_entrada(tabla_de_paginas, paquete_desde_cpu.paginas);
							avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
							free(paquete_desde_cpu.mensaje);
						}
					}
					else { // si no esta en la memoria

						log_acceso_a_swap(logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas);

						if(estan_los_frames_ocupados(tabla_de_paginas->list_pagina_direccion)) {
							pagina_direccion *pagina_ocupada = list_remove(tabla_de_paginas->list_pagina_direccion, 0);

							pagina_direccion *pagina_nueva = malloc(sizeof(pagina_direccion));

							pagina_nueva->en_uso = true;
							pagina_nueva->fue_modificado = false;
							pagina_nueva->nro_pagina = paquete_desde_cpu.paginas;
							pagina_nueva->nro_marco = pagina_ocupada->nro_marco;

							char fifo = 'n';

							list_add(tabla_de_paginas->list_pagina_direccion, pagina_nueva);
							if(config->habilitadaTLB)
								fifo = actualizame_la_tlb(&tlb, paquete_desde_cpu.pid, pagina_nueva->nro_marco * config->tamanio_marco, paquete_desde_cpu.paginas);

							// la pagina ocupada la paso a la swap si esta modificada

							tprotocolo_desde_cpu_y_hacia_swap paquete_a_swap;

							if (pagina_ocupada->fue_modificado) {
								char * mensaje = dame_mensaje_de_memoria(&memoria, pagina_ocupada->nro_marco, config->tamanio_marco);
								armar_estructura_desde_cpu_y_hacia_swap(&paquete_a_swap, 'e', paquete_desde_cpu.pid, pagina_ocupada->nro_pagina, mensaje);
								void* buffer = serializar_a_swap(&paquete_a_swap);
								send(socketClienteSWAP, buffer, strlen(mensaje) + 13, 0);
								free(buffer);
								free(mensaje);
							}

							///////////////////////////////////////////////////////
							// traerse la pagina nueva desde swap
							armar_estructura_desde_cpu_y_hacia_swap(&paquete_a_swap, 'l', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "vacio");
							void * buffer = serializar_a_swap(&paquete_desde_cpu);
							send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
							free(buffer);

							tprotocolo_swap_memoria swap_memoria;
							recibir_paquete_desde_swap(socketClienteSWAP, &swap_memoria);

							//pasar la pagina desde el swap a la memoria
							memcpy(memoria + pagina_nueva->nro_marco * config->tamanio_marco, swap_memoria.mensaje, swap_memoria.tamanio);

							//avisar a la cpu

							char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");
							int nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, pagina_nueva->nro_marco * config->tamanio_marco);

							if(paquete_desde_cpu.cod_op == 'l') {
								log_lectura_escritura('l', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco);

								avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje, socketClienteCPU);
							} else {
								log_lectura_escritura('e', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco);

								memcpy(memoria + nro_marco * config->tamanio_marco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
								pagina_nueva->fue_modificado = true;
								avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
							}
							free(paquete_desde_cpu.mensaje);
							free(pagina_ocupada);
						}
						else { // si hay algun frame libre

							/* asignar un marco libre, bucar en las tablas de paginas de cada proceso y si hay uno libre es porque
							no figura en ninguna tabla de pagina de los proceso*/
							int nro_frame = dame_un_marco_libre(lista_tabla_de_paginas, config->cantidad_marcos);
							if(nro_frame != -1) {
								int i;
								for(i = 0; i < list_size(tabla_de_paginas->list_pagina_direccion) ; i++) {
									pagina_direccion * tabla = list_get(tabla_de_paginas->list_pagina_direccion, i);
									if(!tabla->en_uso) {
										tabla->en_uso = true;
										tabla->fue_modificado = false;
										tabla->nro_pagina = paquete_desde_cpu.paginas;
										tabla->nro_marco = nro_frame;

										char fifo = 'n';
										if(config->habilitadaTLB)
											fifo = actualizame_la_tlb(&tlb, paquete_desde_cpu.pid, tabla->nro_marco * config->tamanio_marco, paquete_desde_cpu.paginas);

										// copiar el contenido del marco de la swap al marco de memoria
										// traerse la pagina nueva desde swap
										tprotocolo_desde_cpu_y_hacia_swap paquete_a_swap;
										armar_estructura_desde_cpu_y_hacia_swap(&paquete_a_swap, 'l', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, paquete_desde_cpu.mensaje);
										void * buffer = serializar_a_swap(&paquete_desde_cpu);
										send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
										free(buffer);

										tprotocolo_swap_memoria swap_memoria;
										recibir_paquete_desde_swap(socketClienteSWAP, &swap_memoria);

										//pasar la pagina desde el swap a la memoria
										memcpy(memoria + nro_frame * config->tamanio_marco, swap_memoria.mensaje, swap_memoria.tamanio);

										char * operacion = fifo == 'n' ? "-" : (fifo == 'e' ? "encontro una entrada en la tlb" : "apĺico fifo en la tlb");
										int nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, tabla->nro_marco * config->tamanio_marco);

										if(paquete_desde_cpu.cod_op == 'l') {
											//avisar a la cpu

											log_lectura_escritura('l', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco);

											char * mensaje = dame_mensaje_de_memoria(&memoria, nro_frame, config->tamanio_marco);
											avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
											free(mensaje);
										} else { // escribir

											log_lectura_escritura('e', operacion ,logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, false, nro_marco);

											memcpy(memoria + nro_marco * config->tamanio_marco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
											tabla->fue_modificado = true;
											avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
										}
										free(paquete_desde_cpu.mensaje);
										break; // cuando la tabla ya esta libre corto con el break el for
									}
								}
							} else {
								// avisale a la cpu que no hay mas memoria y no se puede agregar una nueva entrada de paginas
								avisar_a_cpu(paquete_desde_cpu.cod_op, 'a', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "fallo", socketClienteCPU);
								free(paquete_desde_cpu.mensaje);
							}
						}
					}
				} else { // si esta en la tlb

					int nro_marco = direccion_posta / config->tamanio_marco;
					int nro_tlb = dame_el_numero_de_entrada_de_la_tlb(tlb, direccion_posta);

					log_acceso_memoria(logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_marco);
					if(paquete_desde_cpu.cod_op == 'l') { // leer

						char * mensaje = dame_mensaje_de_memoria(&memoria, nro_marco, config->tamanio_marco);
						log_lectura_escritura('l', "-", logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, true, nro_marco);

						avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
						free(mensaje);

					} else { // escribir, y poner el bit de modificado en true
						memcpy(memoria + direccion_posta, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
						tabla_paginas * tabla_de_paginas = dame_la_tabla_de_paginas(paquete_desde_cpu.pid, &lista_tabla_de_paginas);

						log_lectura_escritura('e', "-", logMem, paquete_desde_cpu.pid, paquete_desde_cpu.paginas, nro_tlb, true, nro_marco);

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
	printf("Ando ...\n");

	/////////////////////////////////////////////////////////////////////////////////////////////
	close(socketClienteSWAP);
	close(socketClienteCPU);
	close(socketServidorCPU);

	pthread_mutex_destroy(&mutex);

	return 0;
}
