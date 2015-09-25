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



void armar_estructura_protocolo(tprotocolo *protocolo, char cod_op, int pid, int paginas, char* mensaje) {

	protocolo->cod_op = cod_op;
	protocolo->pid = pid;
	protocolo->paginas = paginas;
	protocolo->mensaje = malloc(strlen(mensaje) + 1);
	strcpy(protocolo->mensaje, mensaje);
	protocolo->tamanio_mensaje = strlen(protocolo->mensaje) +1;
}

// para usarlo primero uso malloc de la catidad del chorro
void* serializar_a_swap(tprotocolo *protocolo) {

	size_t messageLength = strlen(protocolo->mensaje);
	void * chorro = malloc(13 + messageLength);
	memcpy(chorro, &(protocolo->cod_op), 1);
	memcpy(chorro + 1, &(protocolo->pid), 4);
	memcpy(chorro + 5, &(protocolo->paginas), 4);
	memcpy(chorro + 9, &messageLength, 4);
	memcpy(chorro + 13, protocolo->mensaje, messageLength);
	return chorro;
}

void recibir_paquete_desde_cpu(int socket_cpu, tprotocolo *paquete_desde_cpu) {
	void* buffer = malloc(13);
	recv(socket_cpu, buffer, 13, 0);

	memcpy(&(paquete_desde_cpu->cod_op), buffer ,1);
	memcpy(&(paquete_desde_cpu->pid), buffer + 1 ,4);
	memcpy(&(paquete_desde_cpu->paginas), buffer + 5 ,4);
	memcpy(&(paquete_desde_cpu->tamanio_mensaje), buffer + 9 ,4);
	// ahora el mensaje posta
	paquete_desde_cpu->mensaje = malloc(paquete_desde_cpu->tamanio_mensaje + 1);
	recv(socket_cpu, paquete_desde_cpu->mensaje, paquete_desde_cpu->tamanio_mensaje, 0);
	paquete_desde_cpu->mensaje[paquete_desde_cpu->tamanio_mensaje] = '\0';

}

void recibir_paquete_desde_swap(int socket_swap, tprotocolo_swap_memoria *paquete_desde_swap) {
	void* buffer = malloc(9);
	recv(socket_swap, buffer, 9, 0);

	memcpy(&(paquete_desde_swap->pid), buffer, 4);
	memcpy(&(paquete_desde_swap->error), buffer + 4 ,1);
	memcpy(&(paquete_desde_swap->tamanio), buffer + 5 ,4);
	// ahora el mensaje posta
	paquete_desde_swap->mensaje = malloc(paquete_desde_swap->tamanio + 1);
	recv(socket_swap, paquete_desde_swap->mensaje, paquete_desde_swap->tamanio, 0);
	paquete_desde_swap->mensaje[paquete_desde_swap->tamanio] = '\0';

}


void armar_estructura_protocolo_a_cpu(tprotocolo_memoria_cpu *protocolo, char cod_op, char cod_aux, int pid, int numero_pagina, char* mensaje) {

	protocolo->cod_op = cod_op;
	protocolo->pid = pid;
	protocolo->cod_aux = cod_aux;
	protocolo->numero_pagina = numero_pagina;
	protocolo->mensaje = malloc(strlen(mensaje) + 1);
	strcpy(protocolo->mensaje, mensaje);
	protocolo->tamanio_mensaje = strlen(protocolo->mensaje) +1;
}

void* serializar_a_cpu(tprotocolo_memoria_cpu *protocolo) {

	size_t messageLength = strlen(protocolo->mensaje);
	void * chorro = malloc(14 + messageLength);
	memcpy(chorro, &(protocolo->cod_op), 1);
	memcpy(chorro + 1, &(protocolo->cod_aux), 1);
	memcpy(chorro + 2, &(protocolo->pid), 4);
	memcpy(chorro + 6, &(protocolo->numero_pagina), 4);
	memcpy(chorro + 10, &messageLength, 4);
	memcpy(chorro + 14, protocolo->mensaje, messageLength);
	return chorro;
}


