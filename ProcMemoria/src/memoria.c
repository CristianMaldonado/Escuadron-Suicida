#include <pthread.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include "estructuras.h"
#include <string.h>
#include "paquetes.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "config.h"
#include <stdbool.h>


#define PACKAGESIZE 30


int main(void) {

	system("clear");

	//t_log *logMemoria = log_create("../src/log.txt", "memoria.c", false, LOG_LEVEL_INFO);

	tconfig_memoria * config = leerConfiguracion();

	//Definimos datos Cliente listener
	int socketClienteSWAP;
	printf("Conectando al SWAP (%s : %s)... ", config->ipSwap, config->puertoEscucha);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoEscucha);
	printf("OK\n");

	//log_info(logMemoria, "Conectado al SWAP");

	//Definimos datos Server
	int socketServidorCPU;
	server_init(&socketServidorCPU, "4142");
	printf("Memoria lista...\n");

	//log_info(logMemoria, "Memoria iniciada");

	int socketClienteCPU;
	server_acept(socketServidorCPU, &socketClienteCPU);
	printf("CPU aceptado...\n");
	///////////////////////////////////////////////////////////////////////////////////////////////


	tprotocolo_desde_cpu_y_hacia_swap paquete_desde_cpu;
	recibir_paquete_desde_cpu(socketClienteCPU, &paquete_desde_cpu);

	switch (paquete_desde_cpu.cod_op) {

		case 'f': {
			void* buffer = serializar_a_swap(&paquete_desde_cpu);
			send(socketClienteSWAP, buffer, strlen(paquete_desde_cpu.mensaje) + 13, 0);
			free(buffer);

			tprotocolo_memoria_cpu paquete_memoria_cpu;
			armar_estructura_protocolo_a_cpu(&paquete_memoria_cpu, paquete_desde_cpu.cod_op,'i', paquete_desde_cpu.pid, paquete_desde_cpu.paginas, paquete_desde_cpu.mensaje);
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
			tprotocolo_memoria_cpu memoria_cpu;

			armar_estructura_protocolo_a_cpu(&memoria_cpu, paquete_desde_cpu.cod_op, swap_memoria.error, swap_memoria.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje);
			buffer = serializar_a_cpu(&memoria_cpu);
			send(socketClienteCPU, buffer, strlen(paquete_desde_cpu.mensaje) + 15, 0);

			free(buffer);
		}
		break;
	}

	printf("Finalizo el planificador...\n");



	/////////////////////////////////////////////////////////////////////////////////////////////
	close(socketClienteSWAP);
	//log_info(logMemoria, "cerrada la conexion con Swap");
	close(socketClienteCPU);
	close(socketServidorCPU);
	//log_info(logMemoria, "Memoria finalizada");

	return 0;
}
