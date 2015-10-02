#include <sys/types.h>
#include <sys/socket.h>
#include "libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
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

	printf("Conectando al SWAP (%s : %s)... ", config->ipSwap, config->puertoEscucha);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoEscucha);
	printf("OK\n");

	//Definimos datos Server
	/*server_init(&socketServidorCPU, "4142");
	printf("Memoria lista...\n");

	server_acept(socketServidorCPU, &socketClienteCPU);
	printf("CPU aceptado...\n");
*/




	///////////////////////////////////////////////////////////////////////////////////////

	// mock serializo a memoria:
	tprotocolo_desde_cpu_y_hacia_swap paquete_desde_cpu;
	armar_estructura_desde_cpu_y_hacia_swap(&paquete_desde_cpu, 'i', 9, 3, "hola");

		size_t messageLength = strlen(paquete_desde_cpu.mensaje);
		void * chorro = malloc(13 + messageLength);
		memcpy(chorro, &(paquete_desde_cpu.cod_op), 1);
		memcpy(chorro + 1, &(paquete_desde_cpu.pid), 4);
		memcpy(chorro + 5, &(paquete_desde_cpu.paginas), 4);
		memcpy(chorro + 9, &messageLength, 4);
		memcpy(chorro + 13, paquete_desde_cpu.mensaje, messageLength);


	// recibe la estructura como si fuera del cpu serializada

	//tprotocolo_desde_cpu_y_hacia_swap paquete_desde_cpu;
	recibir_paquete_desde_cpu(&socketClienteCPU, &paquete_desde_cpu);

	printf("\npaquete desde cpu: \n");
	printf("\npid: %d\n", paquete_desde_cpu.pid);
	printf("\ntamanio: %d\n", paquete_desde_cpu.tamanio_mensaje);
	printf("\npaginas: %d\n", paquete_desde_cpu.paginas);
	printf("\ncod_op: %c\n", paquete_desde_cpu.cod_op);
	printf("\nmensaje: %s\n", paquete_desde_cpu.mensaje);




	switch (paquete_desde_cpu.cod_op) {

		case 'f': {
			void* buffer = serializar_a_swap(&paquete_desde_cpu);
			send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
			free(buffer);

			tprotocolo_memoria_cpu paquete_memoria_cpu;
			armar_estructura_protocolo_a_cpu(&paquete_memoria_cpu, paquete_desde_cpu.cod_op, 'f', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, paquete_desde_cpu.mensaje);
			buffer = serializar_a_cpu(&paquete_memoria_cpu);
			send(socketClienteCPU, buffer, strlen(paquete_desde_cpu.mensaje) + 15, 0);

			free(buffer);
		}
		break;

		case 'l':
		case 'i': {
			void* buffer = serializar_a_swap(&paquete_desde_cpu);
			printf("enviar paquete a swap: \n");

			send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
			free(buffer);


			tprotocolo_swap_memoria swap_memoria;
			recibir_paquete_desde_swap(socketClienteSWAP, &swap_memoria);

			printf("\nimprimir paquete desde la swap: \n");
			printf("\npid: %d\n", swap_memoria.pid);
			printf("\ntamanio: %d\n", swap_memoria.tamanio);
			printf("\nmensaje: %s\n", swap_memoria.mensaje);


			tprotocolo_memoria_cpu memoria_cpu;
			//armar_estructura_protocolo_a_cpu(&memoria_cpu, paquete_desde_cpu.cod_op, 'i', swap_memoria.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje);
			//buffer = serializar_a_cpu(&memoria_cpu);
			//send(socketClienteCPU, buffer, strlen(paquete_desde_cpu.mensaje) + 15, 0);

			//free(buffer);
		}
		break;
	}

	printf("Finalizo ...\n");

	/////////////////////////////////////////////////////////////////////////////////////////////


	close(socketClienteSWAP);
	close(socketClienteCPU);
	close(socketServidorCPU);

	return 0;
}
