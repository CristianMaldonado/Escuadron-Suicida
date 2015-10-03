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

	printf("Conectando al SWAP (%s : %s)... ", config->ipSwap, config->puertoSwap);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoSwap);
	printf("OK\n");

	//Definimos datos Server
	server_init(&socketServidorCPU, config->puertoEscucha);
	printf("Memoria lista...\n");

	server_acept(socketServidorCPU, &socketClienteCPU);
	printf("CPU aceptado...\n");





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
			send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
			free(buffer);

			tprotocolo_swap_memoria swap_memoria;
			recibir_paquete_desde_swap(socketClienteSWAP, &swap_memoria);
			printf("\ndesde swap pid %d\n",swap_memoria.pid);
			printf("\ntamanio %d\n",swap_memoria.tamanio);
			printf("\n mensaje %s\n",swap_memoria.mensaje);
			printf("\n codaux %c\n",swap_memoria.codAux);
			tprotocolo_memoria_cpu memoria_cpu;

			armar_estructura_protocolo_a_cpu(&memoria_cpu, paquete_desde_cpu.cod_op, swap_memoria.codAux, swap_memoria.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje);
			buffer = serializar_a_cpu(&memoria_cpu);
			send(socketClienteCPU, buffer, strlen(memoria_cpu.mensaje) + 15, 0);
			free(paquete_desde_cpu.mensaje);
			free(buffer);
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
