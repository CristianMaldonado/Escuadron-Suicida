/*
 * serializacion.c
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */

#include "serializacion.h"
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

void* serializarPaquetePlanificador(protocolo_planificador_cpu* paquete, int* tamanio){ //malloc(1)
	//SERIALIZA SOLO CORRER
	size_t messageLength = strlen(paquete->mensaje);

	void* paqueteSerializado = malloc(sizeof(protocolo_planificador_cpu) + messageLength);
	int offset = 0;
	int size_to_send;

	size_to_send = sizeof(paquete->tipoProceso);
	memcpy(paqueteSerializado + offset, &(paquete->tipoProceso),size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->tipoOperacion);
	memcpy(paqueteSerializado + offset, &(paquete->tipoOperacion),size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->estado);
	memcpy(paqueteSerializado + offset, &(paquete->estado), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->pid);
	memcpy(paqueteSerializado + offset, &(paquete->pid), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->counterProgram);
	memcpy(paqueteSerializado + offset, &(paquete->counterProgram),size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->quantum);
	memcpy(paqueteSerializado + offset, &(paquete->quantum), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->tamanioMensaje);
	memcpy(paqueteSerializado + offset, &messageLength, size_to_send);
	offset += size_to_send;

	size_to_send = messageLength;
	memcpy(paqueteSerializado + offset, paquete->mensaje, size_to_send);
	offset += size_to_send;

	*tamanio = offset;
	return paqueteSerializado;

}

void* serializarPaqueteMemoria(protocolo_cpu_memoria* paquete, int* tamanio) {

	size_t messageLength = strlen(paquete->mensaje);

	void* paqueteSerializado = malloc(sizeof(protocolo_cpu_memoria) + messageLength);
	int offset = 0;
	int size_to_send;

	size_to_send = sizeof(paquete->tipoOperacion);
	memcpy(paqueteSerializado + offset, &(paquete->tipoOperacion),
			size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->pid);
	memcpy(paqueteSerializado + offset, &(paquete->pid), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->nroPagina);
	memcpy(paqueteSerializado + offset, &(paquete->nroPagina), size_to_send);
	offset += size_to_send;

	size_to_send = sizeof(paquete->tamanioMensaje);
	memcpy(paqueteSerializado + offset, &messageLength, size_to_send);
	offset += size_to_send;

	size_to_send = messageLength;
	memcpy(paqueteSerializado + offset, paquete->mensaje, size_to_send);
	offset += size_to_send;

	/*size_to_send = sizeof(paquete->tamanioMensaje);
	memcpy(paqueteSerializado + offset, &(paquete->tamanioMensaje),
			size_to_send);
	offset += size_to_send;

	size_to_send = paquete->tamanioMensaje;
	memcpy(paqueteSerializado + offset, &(paquete->mensaje), size_to_send);
	offset += size_to_send;*/

	*tamanio = offset;
	return paqueteSerializado;

}

int deserializarPlanificador(protocolo_planificador_cpu *package) {
	int status;
	void* buffer = malloc(sizeof(protocolo_planificador_cpu)-4);
	int offset = 0;

	status = recv(socketPlanificador, buffer,sizeof(protocolo_planificador_cpu)-4, 0);

	if(!status) return 0;

	memcpy(&(package->tipoProceso), buffer, sizeof(package->tipoProceso));
	offset += sizeof(package->tipoProceso);

	memcpy(&(package->tipoOperacion), buffer+ offset, sizeof(package->tipoOperacion));
	offset += sizeof(package->tipoOperacion);

	memcpy(&(package->estado), buffer + offset, sizeof(package->estado));
	offset += sizeof(package->estado);

	memcpy(&(package->pid), buffer + offset, sizeof(package->pid));
	offset += sizeof(package->pid);

	memcpy(&(package->counterProgram), buffer + offset,sizeof(package->counterProgram));
	offset += sizeof(package->counterProgram);

	memcpy(&(package->quantum), buffer + offset, sizeof(package->quantum));
	offset += sizeof(package->quantum);

	memcpy(&(package->tamanioMensaje), buffer + offset,sizeof(package->tamanioMensaje));
	offset += sizeof(package->tamanioMensaje);

	package->mensaje = (char*)malloc((package->tamanioMensaje)+1); //valgrind is 1 bytes before a block of size 1 alloc'd

	status = recv(socketPlanificador, package->mensaje, package->tamanioMensaje,0); //valgrind is on thread 1's stack

	package->mensaje[package->tamanioMensaje-1]= '\0';

	free(buffer);

	return status;

}

int deserializarMemoria(protocolo_memoria_cpu* package){
	int status;
		void* buffer = malloc(sizeof(protocolo_memoria_cpu)-4);  //TODO: RESERVAR MEMORIA
		int offset = 0;

		status = recv(socketMemoria, buffer,sizeof(protocolo_memoria_cpu) -4, 0);
		if(!status) return 0;

		memcpy(&(package->tipoProceso), buffer, sizeof(package->tipoProceso));
		offset += sizeof(package->tipoProceso);

		memcpy(&(package->codOperacion), buffer + offset, sizeof(package->codOperacion));
		offset += sizeof(package->codOperacion);

		memcpy(&(package->codAux), buffer + offset, sizeof(package->codAux));
		offset += sizeof(package->codAux);

		memcpy(&(package->pid), buffer + offset, sizeof(package->pid));
		offset += sizeof(package->pid);

		memcpy(&(package->numeroPagina), buffer + offset, sizeof(package->numeroPagina));
		offset += sizeof(package->numeroPagina);

		memcpy(&(package->tamanioMensaje), buffer + offset,sizeof(package->tamanioMensaje));
		offset += sizeof(package->tamanioMensaje);

		package->mensaje = (char*)malloc((package->tamanioMensaje) +1);

		status = recv(socketMemoria, package->mensaje, package->tamanioMensaje,0);

		package->mensaje[package->tamanioMensaje]= '\0';
		/*
		memcpy(&(package->tamanioMensaje), buffer + offset,
				sizeof(package->tamanioMensaje));
		offset += sizeof(package->tamanioMensaje);

		if (!status) return 0;
		package->mensaje = malloc((package->tamanioMensaje) +1);
			status = recv(socketMemoria, buffer, package->tamanioMensaje,0);
			memcpy(&(package->mensaje), buffer + offset, package->tamanioMensaje);
			package->mensaje[package->tamanioMensaje+1]= '\0';*/

		free(buffer);

		return status;
}
