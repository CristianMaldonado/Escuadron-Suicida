/*
 * paquetes.c
 *
 *  Created on: 25/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include "estructuras.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

FILE* iniciar_archivo_swap(void) {
	tconfig_swap *config_swap = leerConfiguracion();

	FILE* swap = fopen(config_swap->nombreSwap, "w+");

	size_t tamanio_swap = config_swap->tamanioPagina * config_swap->cantidadPaginas;

	// rellenamos en cero (char '/0' es 0)
	int i ; char cero = 0;
	for (i = 0 ; i < tamanio_swap; i++)
		fwrite(&cero, sizeof(cero), 1, swap);

	return swap;
}

void recibir_paquete_desde_memoria(int *socket_memoria, tprotocolo_memoria_swap *paquete_desde_memoria) {
	void* buffer = malloc(13);
	recv(*socket_memoria, buffer, 13, 0);
	memcpy(&(paquete_desde_memoria->codigo_op), buffer ,1);
	memcpy(&(paquete_desde_memoria->pid), buffer + 1 ,4);
	memcpy(&(paquete_desde_memoria->cantidad_pagina), buffer + 5 ,4);
	memcpy(&(paquete_desde_memoria->tamanio_mensaje), buffer + 9 ,4);
	// ahora el mensaje posta
	paquete_desde_memoria->mensaje = malloc(paquete_desde_memoria->tamanio_mensaje + 1);
	recv(*socket_memoria, paquete_desde_memoria->mensaje, paquete_desde_memoria->tamanio_mensaje, 0);
	paquete_desde_memoria->mensaje[paquete_desde_memoria->tamanio_mensaje] = '\0';
	free(buffer);
}

// usar un free() despues de enviar el paquete
void* serializar_a_memoria(tprotocolo_swap_memoria *protocolo) {
	size_t messageLength = strlen(protocolo->mensaje);
	void * chorro = malloc(9 + messageLength);
	memcpy(chorro, &(protocolo->pid), 4);
	memcpy(chorro + 4, &(protocolo->error), 1);
	memcpy(chorro + 5, &(protocolo->tamanio), 4);
	memcpy(chorro + 9, protocolo->mensaje, messageLength);
	return chorro;
}


