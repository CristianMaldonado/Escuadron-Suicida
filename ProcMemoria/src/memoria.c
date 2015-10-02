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

int main(void) {
	system("clear");

	//t_log *logMemoria = log_create("../src/log.txt", "memoria.c", false, LOG_LEVEL_INFO);

	tconfig_memoria * config = leerConfiguracion();

	//Definimos datos Cliente listener
	int socketClienteSWAP, socketServidorCPU, socketClienteCPU;
	inicializar_sockets(&socketClienteCPU, &socketClienteSWAP, &socketServidorCPU, config);

	///////////////////////////////////////////////////////////////////////////////////////


	tprotocolo_desde_cpu_y_hacia_swap paquete_desde_cpu;
	recibir_paquete_desde_cpu(&socketClienteCPU, &paquete_desde_cpu);

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
			tprotocolo_memoria_cpu memoria_cpu;

			armar_estructura_protocolo_a_cpu(&memoria_cpu, paquete_desde_cpu.cod_op, 'i', swap_memoria.pid, paquete_desde_cpu.paginas, swap_memoria.mensaje);
			buffer = serializar_a_cpu(&memoria_cpu);
			send(socketClienteCPU, buffer, strlen(paquete_desde_cpu.mensaje) + 15, 0);

			free(buffer);
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
