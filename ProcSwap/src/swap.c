#include <pthread.h>
#include "../../lib/libSocket.h"
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include "estructuras.h"

#define PACKAGESIZE 30


void des_serializar_memoria(void* chorro, tprotocolo_memoria_swap *paquete_desde_memoria);
void iniciar_archivo_swap(void);

int main(void) {
	system("clear");

	// iniciamos en cero el archivo swap
	iniciar_archivo_swap();

	// inicializamos lista de ocupados




	//creacion de la instancia de log
	t_log *logSwap = log_create("../src/log.txt", "swap.c", false, LOG_LEVEL_INFO);











	/*Crea el socket para escuchar*/
	int serverSocket;
	server_init(&serverSocket, "4141");
	printf("SWAP listo...\n");

	// loguea Swap iniciado
	log_info(logSwap, "SWAP iniciado");

	/*Crea el socket para recibir a la memoria*/
	int socketMemoria;
	server_acept(serverSocket, &socketMemoria);
	printf("Memoria aceptada...\n");

	// loguea conexion con Memoria
	log_info(logSwap, "Conectado a la memoria");


	/*
	//Pasaje de mensaje
	char package[PACKAGESIZE];
	int status = 1;

	while (status) {
		status = recv(socketMemoria, (void*) package, PACKAGESIZE, 0);
		if (status)
			printf("%s", package);
	}
*/


































	printf("Finalizo el planificador...\n");

	close(serverSocket);
	log_info(logSwap, "SWAP finalizado");

	close(socketMemoria);
	log_info(logSwap, "Cerrada conexion con memoria");

	return 0;
}



void iniciar_archivo_swap(void) {
	tconfig_swap *config_swap = leerConfiguracion();

	FILE* swap = fopen(config_swap->nombreSwap, "w+");

	size_t tamanio_swap = config_swap->tamanioPagina * config_swap->cantidadPaginas;

	// rellenamos en cero (char '/0' es 0)
	int i ; char cero = 0;
	for (i = 0 ; i < tamanio_swap; i++)
		fwrite(&cero, sizeof(cero), 1, swap);

	fclose(swap);
}

void des_serializar_memoria(void* chorro, tprotocolo_memoria_swap *paquete_desde_memoria) {

	memcpy(&(paquete_desde_memoria->codigo_op), chorro ,1);
	memcpy(&(paquete_desde_memoria->pid), chorro + 1, 4);
	memcpy(&(paquete_desde_memoria->cantidad_pagina), chorro + 5, 4);
	memcpy(&(paquete_desde_memoria->tamanio_mensaje), chorro + 9, 4);

	paquete_desde_memoria->mensaje = malloc(paquete_desde_memoria->tamanio_mensaje + 1);
	memcpy(paquete_desde_memoria->mensaje, chorro + 13, paquete_desde_memoria->tamanio_mensaje);
	paquete_desde_memoria->mensaje[paquete_desde_memoria->tamanio_mensaje] = '\0';

}

