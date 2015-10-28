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
#include "config.h"
#include <stdbool.h>
#include <unistd.h>
#include "funciones_memoria.h"





int main(void) {
	system("clear");

	//t_log *logMemoria = log_create("../src/log.txt", "memoria.c", false, LOG_LEVEL_INFO);

	tconfig_memoria * config = leerConfiguracion();

	//Definimos datos Cliente listener
	int socketClienteSWAP, socketServidorCPU, socketClienteCPU;

	printf("Conectando al SWAP (%s : %s)... ", config->ipSwap, config->puertoSwap);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoSwap);
	printf("OK\n");

	//Definimos datos Server
	server_init(&socketServidorCPU, config->puertoEscucha);
	printf("Memoria lista...\n");

	server_acept(socketServidorCPU, &socketClienteCPU);
	printf("CPU aceptado...\n");





	///////////////////////////////////////////////////////////////////////////////////////
	// creamos la representacion memoria

	char * memoria = crear_memoria(config->cantidadMarcos, config->tamanioMarco);

	t_list * lista_tabla_de_paginas = list_create();

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
			void* buffer = serializar_a_swap(&paquete_desde_cpu);
			send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
			free(buffer);

			tprotocolo_swap_memoria swap_memoria;
			recibir_paquete_desde_swap(socketClienteSWAP, &swap_memoria);

			if(swap_memoria.codAux != 'a')
				list_add(lista_tabla_de_paginas, inicializar_tabla_de_paginas(config->maximoMarcosPorProceso, paquete_desde_cpu.pid));

			avisar_a_cpu(paquete_desde_cpu.cod_op, swap_memoria.codAux, swap_memoria.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje, socketClienteCPU);

			free(paquete_desde_cpu.mensaje);

		}
		break;


			case 'f': {
			void* buffer = serializar_a_swap(&paquete_desde_cpu);
			send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
			free(buffer);

			eliminar_tabla_de_proceso(paquete_desde_cpu.pid, &lista_tabla_de_paginas);

			tprotocolo_memoria_cpu paquete_memoria_cpu;
			armar_estructura_protocolo_a_cpu(&paquete_memoria_cpu, paquete_desde_cpu.cod_op, 'f', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, paquete_desde_cpu.mensaje);
			buffer = serializar_a_cpu(&paquete_memoria_cpu);
			send(socketClienteCPU, buffer, strlen(paquete_desde_cpu.mensaje) + 15, 0);

			free(buffer);
		}
		break;

		case 'e':
		case 'l': {

			tabla_paginas *tabla_de_paginas = dame_la_tabla_de_paginas(paquete_desde_cpu.pid, &lista_tabla_de_paginas);

			// paginas es numero de pagina o cantidad de paginas depende el protocolo, en este caso es numero de pagina
			int nro_marco = dame_la_direccion_de_la_pagina(tabla_de_paginas, paquete_desde_cpu.paginas);

			// es valida o no la direccion
			if(nro_marco != -1) {
				if(paquete_desde_cpu.cod_op == 'l') {

					char * mensaje = dame_mensaje_de_memoria(&memoria, nro_marco, config->tamanioMarco);

					avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
						/*
					tprotocolo_memoria_cpu memoria_cpu;
					armar_estructura_protocolo_a_cpu(&memoria_cpu, paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje);
					void * buffer;
					buffer = serializar_a_cpu(&memoria_cpu);
					send(socketClienteCPU, buffer, strlen(mensaje) + 15, 0);*/
					free(paquete_desde_cpu.mensaje);
					free(mensaje);
				} else {
					memcpy(memoria + nro_marco * config->tamanioMarco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
					avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
					free(paquete_desde_cpu.mensaje);
				}
			}
			else {

				if(estan_los_frames_ocupados(tabla_de_paginas->list_pagina_direccion)) {
					pagina_direccion *pagina_ocupada = list_remove(tabla_de_paginas->list_pagina_direccion, 0);

					pagina_direccion *pagina_nueva = malloc(sizeof(pagina_direccion));

					pagina_nueva->en_uso = true;
					pagina_nueva->fue_modificado = false;
					pagina_nueva->nro_pagina = paquete_desde_cpu.paginas;
					pagina_nueva->nro_marco = pagina_ocupada->nro_marco;

					list_add(tabla_de_paginas->list_pagina_direccion, pagina_nueva);

					///////////////////////////////////////////////////////
					// la pagina ocupada la paso a la swap

					char * mensaje = dame_mensaje_de_memoria(&memoria, pagina_ocupada->nro_marco, config->tamanioMarco);

					tprotocolo_desde_cpu_y_hacia_swap paquete_a_swap;

					armar_estructura_desde_cpu_y_hacia_swap(&paquete_a_swap, 'e', paquete_desde_cpu.pid, pagina_ocupada->nro_pagina, mensaje);
					void* buffer = serializar_a_swap(&paquete_a_swap);
					send(socketClienteSWAP, buffer, strlen(mensaje) + 13, 0);
					free(buffer);
					free(mensaje);
					///////////////////////////////////////////////////////
					// traerse la pagina nueva desde swap
					armar_estructura_desde_cpu_y_hacia_swap(&paquete_a_swap, 'l', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "vacio");
					buffer = serializar_a_swap(&paquete_desde_cpu);
					send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
					free(buffer);

					tprotocolo_swap_memoria swap_memoria;
					recibir_paquete_desde_swap(socketClienteSWAP, &swap_memoria);


					//pasar la pagina desde el swap a la memoria
					memcpy(memoria + pagina_nueva->nro_marco * config->tamanioMarco, swap_memoria.mensaje, swap_memoria.tamanio);

					//avisar a la cpu
					if(paquete_desde_cpu.cod_op == 'l') {
						avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje, socketClienteCPU);
					} else {
						memcpy(memoria + nro_marco * config->tamanioMarco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
						avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
					}

					free(paquete_desde_cpu.mensaje);
					free(pagina_ocupada);
				}
				else {
					// asignar un marco libre, bucar en las tablas de paginas de cada proceso y si hay uno libre es porque
					// no figura en ninguna tabla de pagina de los proceso
					int nro_frame = dame_un_marco_libre(lista_tabla_de_paginas, config->tamanioMarco);
					if(nro_frame != -1) {
						int i;
						for(i = 0; i < list_size(tabla_de_paginas->list_pagina_direccion) ; i++) {
							pagina_direccion * tabla = list_get(tabla_de_paginas->list_pagina_direccion, i);
							if(!tabla->en_uso) {
								tabla->en_uso = true;
								tabla->fue_modificado = false;
								tabla->nro_pagina = paquete_desde_cpu.paginas;
								tabla->nro_marco = nro_frame;

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

								memcpy(memoria + nro_frame * config->tamanioMarco, swap_memoria.mensaje, swap_memoria.tamanio);

								if(paquete_desde_cpu.cod_op == 'l') {
									//avisar a la cpu
									char * mensaje = dame_mensaje_de_memoria(&memoria, nro_frame, config->tamanioMarco);
									avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje, socketClienteCPU);
									free(mensaje);
								} else { // escribir
									memcpy(memoria + nro_marco * config->tamanioMarco, paquete_desde_cpu.mensaje, paquete_desde_cpu.tamanio_mensaje);
									avisar_a_cpu(paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "nada", socketClienteCPU);
								}
								free(paquete_desde_cpu.mensaje);
							}
						}
					} else {
						// avisale a la cpu que fallo leer o escribir
						avisar_a_cpu(paquete_desde_cpu.cod_op, 'a', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, "fallo", socketClienteCPU);
						free(paquete_desde_cpu.mensaje);
					}
				}
			}
			/*
			void* buffer = serializar_a_swap(&paquete_desde_cpu);
			send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
			free(buffer);


			tprotocolo_swap_memoria swap_memoria;
			recibir_paquete_desde_swap(socketClienteSWAP, &swap_memoria);

			tprotocolo_memoria_cpu memoria_cpu;

			armar_estructura_protocolo_a_cpu(&memoria_cpu, paquete_desde_cpu.cod_op, swap_memoria.codAux, swap_memoria.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje);
			buffer = serializar_a_cpu(&memoria_cpu);
			send(socketClienteCPU, buffer, strlen(memoria_cpu.mensaje) + 15, 0);
			free(paquete_desde_cpu.mensaje);
			free(buffer);
			*/
		}
		break;

		}
	}
	printf("Finalizo ...\n");

	/////////////////////////////////////////////////////////////////////////////////////////////

	close(socketClienteSWAP);
	close(socketClienteCPU);
	close(socketServidorCPU);

	return 0;
}
