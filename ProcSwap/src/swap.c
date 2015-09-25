#include <pthread.h>
#include "../../lib/libSocket.h"
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include "estructuras.h"
#include "paquetes.h"

#define PACKAGESIZE 30




int main(void) {
	system("clear");

	// iniciamos en cero el archivo swap
//	iniciar_archivo_swap();

	// inicializamos lista de ocupados




	//creacion de la instancia de log
	//t_log *logSwap = log_create("../src/log.txt", "swap.c", false, LOG_LEVEL_INFO);











	/*Crea el socket para escuchar*/
	int serverSocket;
	server_init(&serverSocket, "4141");
	printf("SWAP listo...\n");

	// loguea Swap iniciado
	//log_info(logSwap, "SWAP iniciado");

	/*Crea el socket para recibir a la memoria*/
	int socketMemoria;
	server_acept(serverSocket, &socketMemoria);
	printf("Memoria aceptada...\n");

	// loguea conexion con Memoria
	//log_info(logSwap, "Conectado a la memoria");




	// pasaje de mensaje prueba recibir un chorro

	tprotocolo_memoria_swap *prot;
	recibir_paquete_desde_memoria(socketMemoria, prot);
	printf("%c\n", prot->codigo_op);
	printf("%d\n", prot->cantidad_pagina);
	printf("%d\n", prot->pid);
	printf("%d\n", prot->tamanio_mensaje);
	printf("%s\n", prot->mensaje);


	/*
	//Pasaje de mensaje checkpoint 1
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
	//log_info(logSwap, "SWAP finalizado");

	close(socketMemoria);
	//log_info(logSwap, "Cerrada conexion con memoria");

	return 0;
}
