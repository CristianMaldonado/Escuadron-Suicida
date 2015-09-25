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


void des_serializar_cpu(void* buffer, tprotocolo *paquete_Desde_Cpu) {

	//desde el buffer tomo parte por parte y lo copio en la estructura
	memcpy(&(paquete_Desde_Cpu->cod_op), buffer ,1 );
	memcpy(&(paquete_Desde_Cpu->pid), buffer + 1, 4);
	memcpy(&(paquete_Desde_Cpu->paginas), buffer + 5, 4);
	memcpy(&(paquete_Desde_Cpu->tamanio_mensaje), buffer + 9, 4);

	paquete_Desde_Cpu->mensaje = malloc(paquete_Desde_Cpu->tamanio_mensaje + 1);
	memcpy(paquete_Desde_Cpu->mensaje, buffer + 13, paquete_Desde_Cpu->tamanio_mensaje);
	paquete_Desde_Cpu->mensaje[paquete_Desde_Cpu->tamanio_mensaje] = '\0';
}


