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
#include <stdbool.h>

FILE* iniciar_archivo_swap(void) {
	tconfig_swap *config_swap = leerConfiguracion();
	FILE* swap = fopen(config_swap->nombreSwap, "wb+"); //rb+ para archivo binario
	size_t tamanio_swap = config_swap->tamanioPagina * config_swap->cantidadPaginas;
	//rellenamos en cero (char '/0' es 0)
	int i ;
	char cero[] = "0";
	for (i = 0 ; i < tamanio_swap; i++)
		fwrite(cero, sizeof(char), 1, swap);
	fseek(swap, 0, SEEK_SET);
	return swap;

}

bool recibir_paquete_desde_memoria(int *socket_memoria, tprotocolo_memoria_swap *paquete_desde_memoria) {
	void* buffer = malloc(13 * sizeof(int));
	if (recv(*socket_memoria, buffer, 13, 0) <= 0) return false;
	memcpy(&(paquete_desde_memoria->codigo_op), buffer ,1);
	memcpy(&(paquete_desde_memoria->pid), buffer + 1 ,4);
	memcpy(&(paquete_desde_memoria->cantidad_pagina), buffer + 5 ,4);
	memcpy(&(paquete_desde_memoria->tamanio_mensaje), buffer + 9 ,4);
	// ahora el mensaje posta
	paquete_desde_memoria->mensaje = (char*)malloc(paquete_desde_memoria->tamanio_mensaje + 1);
	if(recv(*socket_memoria, paquete_desde_memoria->mensaje, paquete_desde_memoria->tamanio_mensaje, 0) <= 0) return false;
	paquete_desde_memoria->mensaje[paquete_desde_memoria->tamanio_mensaje] = '\0';
	free(buffer);
	return true;
}


void* serializar_a_memoria(tprotocolo_swap_memoria *protocolo) {
	size_t messageLength = strlen(protocolo->mensaje);
	void * chorro = malloc(8 + messageLength);
	memcpy(chorro, &(protocolo->pid), 4);
	memcpy(chorro + 4, &(protocolo->tamanio), 4);
	memcpy(chorro + 8, protocolo->mensaje, messageLength);
	return chorro;
}


void armar_estructura_protocolo_a_memoria(tprotocolo_swap_memoria *protocolo, int pid, char* mensaje) {
	protocolo->pid = pid;
	protocolo->mensaje = malloc(strlen(mensaje) + 1);
	strcpy(protocolo->mensaje, mensaje);
	protocolo->tamanio = strlen(protocolo->mensaje) +1;
}





