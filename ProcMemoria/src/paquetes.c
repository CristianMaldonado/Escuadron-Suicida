/*
 * paquetes.c
 *
 *  Created on: 25/9/2015
 *      Author: utnso
 */

#include "estructuras.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>


void armar_estructura_desde_cpu_y_hacia_swap(tprotocolo_desde_cpu_y_hacia_swap *protocolo, char cod_op, int pid, int paginas, char* mensaje) {
	protocolo->cod_op = cod_op;
	protocolo->pid = pid;
	protocolo->paginas = paginas;
	protocolo->mensaje = malloc(strlen(mensaje) + 1);
	strcpy(protocolo->mensaje, mensaje);
	protocolo->tamanio_mensaje = strlen(protocolo->mensaje);
}

void* serializar_a_swap(tprotocolo_desde_cpu_y_hacia_swap *protocolo) {
	size_t messageLength = strlen(protocolo->mensaje);
	void * chorro = malloc(13 + messageLength);
	memcpy(chorro, &(protocolo->cod_op), 1);
	memcpy(chorro + 1, &(protocolo->pid), 4);
	memcpy(chorro + 5, &(protocolo->paginas), 4);
	memcpy(chorro + 9, &messageLength, 4);
	memcpy(chorro + 13, protocolo->mensaje, messageLength);
	return chorro;
}

bool recibir_paquete_desde_cpu(int *socket_cpu, tprotocolo_desde_cpu_y_hacia_swap *paquete_desde_cpu) {
	void* buffer = malloc(13 * sizeof(int));
	if(recv(*socket_cpu, buffer, 13, 0) <= 0) return false;
	memcpy(&(paquete_desde_cpu->cod_op), buffer ,1);
	memcpy(&(paquete_desde_cpu->pid), buffer + 1 ,4);
	memcpy(&(paquete_desde_cpu->paginas), buffer + 5 ,4);
	memcpy(&(paquete_desde_cpu->tamanio_mensaje), buffer + 9 ,4);
	// ahora el mensaje posta
	paquete_desde_cpu->mensaje = (char*)malloc(paquete_desde_cpu->tamanio_mensaje + 1);
	if(recv(*socket_cpu, paquete_desde_cpu->mensaje, paquete_desde_cpu->tamanio_mensaje, 0) <= 0) return false;
	paquete_desde_cpu->mensaje[paquete_desde_cpu->tamanio_mensaje] = '\0';
	free(buffer);
	return true;
}

bool recibir_paquete_desde_swap(int socket_swap, tprotocolo_swap_memoria *paquete_desde_swap) {
	void* buffer = malloc(8 * sizeof(int));
	if(recv(socket_swap, buffer, 8, 0) <= 0) return false;
	memcpy(&(paquete_desde_swap->pid), buffer, 4);
	memcpy(&(paquete_desde_swap->tamanio), buffer + 4, 4);
	// ahora el mensaje posta
	paquete_desde_swap->mensaje = (char*)malloc(paquete_desde_swap->tamanio + 1);
	if(recv(socket_swap, paquete_desde_swap->mensaje, paquete_desde_swap->tamanio, 0) <= 0) return false;
	paquete_desde_swap->mensaje[paquete_desde_swap->tamanio] = '\0';
	free(buffer);
	return true;
}

void armar_estructura_protocolo_a_cpu(tprotocolo_memoria_cpu *protocolo, char cod_op, char cod_aux, int pid, int numero_pagina, char* mensaje) {
	protocolo->cod_proceso = 'm';
	protocolo->cod_op = cod_op;
	protocolo->pid = pid;
	protocolo->cod_aux = cod_aux;
	protocolo->numero_pagina = numero_pagina;
	protocolo->mensaje = malloc(strlen(mensaje)+1);
	strcpy(protocolo->mensaje, mensaje);
	protocolo->tamanio_mensaje = strlen(mensaje);
}

void* serializar_a_cpu(tprotocolo_memoria_cpu *protocolo) {
	size_t messageLength = strlen(protocolo->mensaje);
	void * chorro = malloc(15 + messageLength);
	memcpy(chorro, &(protocolo->cod_proceso), 1);
	memcpy(chorro + 1, &(protocolo->cod_op), 1);
	memcpy(chorro + 2, &(protocolo->cod_aux), 1);
	memcpy(chorro + 3, &(protocolo->pid), 4);
	memcpy(chorro + 7, &(protocolo->numero_pagina), 4);
	memcpy(chorro + 11, &messageLength, 4);
	memcpy(chorro + 15, protocolo->mensaje, messageLength);
	return chorro;
}


