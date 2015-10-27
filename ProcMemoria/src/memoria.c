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

	printf("estoy por recibir\n");
	printf("cod op desde cpu %c\n",paquete_desde_cpu.cod_op);
	printf("paginas %d\n",paquete_desde_cpu.paginas);
	printf("tamanio %d\n",paquete_desde_cpu.tamanio_mensaje);
	printf("pid %d\n",paquete_desde_cpu.pid);
	printf("mensaje %s\n",paquete_desde_cpu.mensaje);
	switch (paquete_desde_cpu.cod_op) {

		case 'i': {
			void* buffer = serializar_a_swap(&paquete_desde_cpu);
			send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
			free(buffer);

			tprotocolo_swap_memoria swap_memoria;
			recibir_paquete_desde_swap(socketClienteSWAP, &swap_memoria);

			if(swap_memoria.codAux != 'a')
				list_add(lista_tabla_de_paginas, inicializar_tabla_de_paginas(config->maximoMarcosPorProceso, paquete_desde_cpu.pid));


			tprotocolo_memoria_cpu memoria_cpu;

			armar_estructura_protocolo_a_cpu(&memoria_cpu, paquete_desde_cpu.cod_op, swap_memoria.codAux, swap_memoria.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje);
			buffer = serializar_a_cpu(&memoria_cpu);
			send(socketClienteCPU, buffer, strlen(memoria_cpu.mensaje) + 15, 0);
			free(paquete_desde_cpu.mensaje);
			free(buffer);
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

		case 'l': {

			tabla_paginas *tabla_de_paginas = dame_la_tabla_de_paginas(paquete_desde_cpu.pid, &lista_tabla_de_paginas);

			// paginas es numero de pagina o cantidad de paginas depende el protocolo, en este caso es numero de pagina
			int direccion = dame_la_direccion_de_la_pagina(tabla_de_paginas, paquete_desde_cpu.paginas);

			// es valida o no la direccion
			if(direccion != -1) {
				tprotocolo_memoria_cpu memoria_cpu;

				char *mensaje_memoria = memoria + direccion * config->tamanioMarco;
				char *mensaje = malloc(config->tamanioMarco + 1);
				memcpy(mensaje, mensaje_memoria, config->tamanioMarco);
				mensaje[config->tamanioMarco] = '\0';

				armar_estructura_protocolo_a_cpu(&memoria_cpu, paquete_desde_cpu.cod_op, 'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, mensaje);
				void * buffer;
				buffer = serializar_a_cpu(&memoria_cpu);
				send(socketClienteCPU, buffer, strlen(memoria_cpu.mensaje) + 15, 0);
				free(paquete_desde_cpu.mensaje);
				free(buffer);
			}
			else {

				if(estan_los_frames_ocupados(tabla_de_paginas->list_pagina_direccion)) {
					pagina_direccion *pagina_ocupada = list_remove(tabla_de_paginas.list_pagina_direccion, 0);

					pagina_direccion *pagina_nueva = malloc(sizeof(pagina_direccion));

					pagina_nueva->en_uso = true;
					pagina_nueva->fue_modificado = false;
					pagina_nueva->nro_pagina = paquete_desde_cpu.paginas;
					pagina_nueva->nro_marco = pagina_ocupada->nro_marco;

					///////////////////////////////////////////////////////
					// si la pagina ocupada esta modificada la paso a la swap





					///////////////////////////////////////////////////////

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
