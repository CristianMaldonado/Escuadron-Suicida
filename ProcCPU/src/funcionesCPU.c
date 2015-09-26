/*
 * funcionesCPU.c
 *
 *  Created on: 23/9/2015
 *      Author: utnso
 */

#include "funcionesCPU.h"
#include <commons/error.h>
#include "estructuras.h"
#include <stdlib.h>
#include <string.h>
#include<sys/socket.h>

char* serializarPaquete(protocolo_cpu_memoria* paquete) {
	char* paqueteSerializado = malloc(sizeof(paquete));
	int offset = 0;
	int size_to_send;

	size_to_send = sizeof(paquete->tipoProceso);
	memcpy(paqueteSerializado + offset, &(paquete->tipoProceso), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->tipoOperacion);
	memcpy(paqueteSerializado + offset, &(paquete->tipoOperacion), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->pid);
	memcpy(paqueteSerializado + offset, &(paquete->pid), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->nroPagina);
	memcpy(paqueteSerializado + offset, &(paquete->nroPagina), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->tamanioMensaje);
	memcpy(paqueteSerializado + offset, &(paquete->tamanioMensaje),
			size_to_send);
	offset += size_to_send;

	size_to_send = paquete->tamanioMensaje;
	memcpy(paqueteSerializado + offset, &(paquete->mensaje), size_to_send);
	offset += size_to_send;

	return paqueteSerializado;

}

void enviar(tMensajeAMemoria* message) {
	//protocolo_cpu_memoria* paquete = armarPaquete(message->lineaDeProceso);
	char* empaquetado = serializarPaquete(message->paquete); //TODO : REVISAR ARMAR PAQUETE (PARAM A ENVIAR)
	send(message->socketMemoria, empaquetado, sizeof(empaquetado), 0);
	sem_post(&ejecutaInstruccion);
}


void interpretarInstruccion(tMensajeAMemoria* message) {

	char** linea = string_n_split(message->lineaDeProceso, 3, " ");
	if (string_starts_with(message->lineaDeProceso, "iniciar")) {
		armarPaquete(message->paquete, 'c', 'i', message->pid, atoi(linea[1]),"\0");
		//enviar(message);
	}
	if (string_starts_with(message->lineaDeProceso, "leer")) {
		enviar(message);
	}
	//if(string_starts_with(message->lineaDeProceso,"escribir")) { enviar(message); }
	//if(string_starts_with(message->lineaDeProceso,"entrada-salida")) { enviar(message); }
	if (string_starts_with(message->lineaDeProceso, "finalizar")) {
		enviar(message);
	}
}


//MODIFICAR ARMAR PAQUETE PARAMETROS
void armarPaquete(protocolo_cpu_memoria* paquete,char tipoProceso, char codOperacion, int pid, int nroPagina, char* mensaje) {

	paquete->tipoProceso = tipoProceso;
	paquete->tipoOperacion = codOperacion;
	paquete->pid = pid;
	paquete->nroPagina = nroPagina;
	paquete->tamanioMensaje = strlen(mensaje)+1;
	strcpy(paquete->mensaje, mensaje);

	//TODO Hacerlo mas genérico con un booleano y cargue la estructura (sin mandar todos los parametros)
}


void liberar_paquete(char **paquete){
	free(*paquete);
}
