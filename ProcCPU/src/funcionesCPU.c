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
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void liberar_paquete(char **paquete) {
	free(*paquete);
}


void crearMockitoPlanif(protocolo_planificador_cpu* package){

	package->tipoProceso = 'P';
	package->tipoOperacion = 'l';
	package->estado = EJECUTANDO;
	package->pid = 1;
	package->counterProgram = 2;
	package->quantum = 0;
	package->mensaje = malloc(strlen("programa.txt")+1);
	strcpy(package->mensaje,"programa.txt");
	package->tamanioMensaje = strlen(package->mensaje)+1;

}

char* serializarPaqueteMemoria(protocolo_cpu_memoria* paquete) { //malloc(1)
	char* paqueteSerializado = malloc(sizeof(paquete));
	int offset = 0;
	int size_to_send;

	size_to_send = sizeof(paquete->tipoProceso);
	memcpy(paqueteSerializado + offset, &(paquete->tipoProceso), size_to_send);
	offset += size_to_send;

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
	memcpy(paqueteSerializado + offset, &(paquete->tamanioMensaje),
			size_to_send);
	offset += size_to_send;

	size_to_send = paquete->tamanioMensaje;
	memcpy(paqueteSerializado + offset, &(paquete->mensaje), size_to_send);
	offset += size_to_send;

	return paqueteSerializado;

}

int deserializarPlanificador(protocolo_planificador_cpu *package,
		int socketPlanificador) { //TODO deserializar mensaje de planificador
	int status;
	char* buffer = malloc(sizeof(package->tipoProceso)+ sizeof(package->tipoOperacion)+ sizeof(testado)+ sizeof(package->pid)+
			sizeof(package->counterProgram)+ sizeof(package->quantum)+ sizeof(package->tamanioMensaje));
	int offset = 0;

	status = recv(socketPlanificador, buffer,
			sizeof(package->tipoOperacion) + sizeof(package->tipoProceso), 0);
	memcpy(&(package->tipoProceso), buffer, sizeof(package->tipoProceso));
	offset += sizeof(package->tipoProceso);
	memcpy(&(package->tipoOperacion), buffer + offset, sizeof(package->tipoOperacion));
	offset += sizeof(package->tipoOperacion);

	if (!status) return 0;

	status = recv(socketPlanificador, buffer,
			sizeof(package->estado) + sizeof(package->pid)
					+ sizeof(package->counterProgram + sizeof(package->quantum)
							+ sizeof(package->tamanioMensaje)),0);
	memcpy(&(package->estado), buffer + offset, sizeof(package->estado));
	offset += sizeof(package->estado);
	memcpy(&(package->pid), buffer + offset, sizeof(package->pid));
	offset += sizeof(package->pid);
	memcpy(&(package->counterProgram), buffer + offset,
			sizeof(package->counterProgram));
	offset += sizeof(package->counterProgram);
	memcpy(&(package->quantum), buffer + offset, sizeof(package->quantum));
	offset += sizeof(package->quantum);
	memcpy(&(package->tamanioMensaje), buffer + offset,
			sizeof(package->tamanioMensaje));
	offset += sizeof(package->tamanioMensaje);

	if (!status) return 0;

    package->mensaje = malloc((package->tamanioMensaje) +1);
	status = recv(socketPlanificador, buffer, package->tamanioMensaje,0);
	memcpy(&(package->mensaje), buffer + offset, package->tamanioMensaje);
	package->mensaje[package->tamanioMensaje+1]= '\0';
	if(!status) return 0;

	free(buffer);

	return status;

}

int deserializarMemoria(protocolo_memoria_cpu* package,int socketMemoria){
	int status;
		char* buffer = malloc(sizeof(package->codOperacion) + sizeof(package->tipoProceso) + sizeof(package->codAux)+
				              sizeof(package->pid)+ sizeof(package->numeroPagina)+ sizeof(package->tamanioMensaje));  //TODO: RESERVAR MEMORIA
		int offset = 0;

		status = recv(socketMemoria, buffer,
				sizeof(package->codOperacion) + sizeof(package->tipoProceso) + sizeof(package->codAux), 0);
		memcpy(&(package->tipoProceso), buffer, sizeof(package->tipoProceso));
		offset += sizeof(package->tipoProceso);
		memcpy(&(package->codOperacion), buffer + offset, sizeof(package->codOperacion));
		offset += sizeof(package->codOperacion);
		memcpy(&(package->codAux), buffer + offset, sizeof(package->codAux));
		offset += sizeof(package->codAux);
		if (!status) return 0;

		status = recv(socketMemoria, buffer,
				sizeof(package->pid) + sizeof(package->numeroPagina)
						+ sizeof(package->tamanioMensaje),0);
		memcpy(&(package->pid), buffer + offset, sizeof(package->pid));
		offset += sizeof(package->pid);
		memcpy(&(package->numeroPagina), buffer + offset, sizeof(package->numeroPagina));
		offset += sizeof(package->numeroPagina);
		memcpy(&(package->tamanioMensaje), buffer + offset,
				sizeof(package->tamanioMensaje));
		offset += sizeof(package->tamanioMensaje);
		if (!status) return 0;
		package->mensaje = malloc((package->tamanioMensaje) +1);
			status = recv(socketMemoria, buffer, package->tamanioMensaje,0);
			memcpy(&(package->mensaje), buffer + offset, package->tamanioMensaje);
			package->mensaje[package->tamanioMensaje+1]= '\0';
			if(!status) return 0;

		free(buffer);

		return status;
}
void enviar(tParametroHilo* message) {
	char* empaquetado = serializarPaqueteMemoria(message->mensajeAMemoria);
	send(message->socketMemoria, empaquetado, string_length(empaquetado) + 1,
			0);
	liberar_paquete(&empaquetado); //free(1)
}


void interpretarInstruccion(char* instruccion, tParametroHilo* mensajeParaArmar) {

	char** linea = string_split(instruccion, " ");
	if (string_starts_with(instruccion, "iniciar")) {
		armarPaquete(mensajeParaArmar->mensajeAMemoria, 'c', 'i',
				mensajeParaArmar->mensajeAPlanificador->pid, atoi(linea[1]),
				"\0");
	}
	if (string_starts_with(instruccion, "leer")) {
		armarPaquete(mensajeParaArmar->mensajeAMemoria, 'c', 'l',
				mensajeParaArmar->mensajeAPlanificador->pid, atoi(linea[1]),
				"\0");
	}
	//if(string_starts_with(message->lineaDeProceso,"escribir")) {  } //TODO cheackpoint 3 supongo
	//if(string_starts_with(message->lineaDeProceso,"entrada-salida")) { }
	if (string_starts_with(instruccion, "finalizar")) {
		armarPaquete(mensajeParaArmar->mensajeAMemoria, 'c', 'f',
				mensajeParaArmar->mensajeAPlanificador->pid, 0, "\0");
	}
}

//MODIFICAR ARMAR PAQUETE PARAMETROS
void armarPaquete(protocolo_cpu_memoria* paquete, char tipoProceso,
		char codOperacion, int pid, int nroPagina, char* mensaje) {
	paquete->tipoProceso = tipoProceso;
	paquete->tipoOperacion = codOperacion;
	paquete->pid = pid;
	paquete->nroPagina = nroPagina;
	paquete->tamanioMensaje = strlen(mensaje) + 1;
	strcpy(paquete->mensaje, mensaje);

	//TODO Hacerlo mas genérico con un booleano y cargue la estructura (sin mandar todos los parametros)
}

char* leerInstruccion(int* instructionPointer,char* lineaLeida, FILE* archivo) {	//ruta+instruction pointer => leo la linea del ip y la devuelvo


	int cont = 1;
	if (*instructionPointer == 1) {
		fgets(lineaLeida, ftell(archivo), archivo);
		(*instructionPointer) = (*instructionPointer) + 1;
	}

	while (!feof(archivo) && cont != (*instructionPointer)) {
		fgets(lineaLeida, ftell(archivo), archivo);
		cont++;
		(*instructionPointer) = (*instructionPointer) + 1;
	}

	return lineaLeida;

}

void cargarParametrosHilo(int socketPlanificador, int socketMemoria,
		protocolo_planificador_cpu* mensajeDePlanif, tParametroHilo* parametros) {
    parametros->mensajeAPlanificador = malloc(sizeof(char)*2 + sizeof(int)*5 + (mensajeDePlanif->tamanioMensaje));
	parametros->socketMemoria = socketMemoria;
	parametros->socketPlanificador = socketPlanificador;
	parametros->mensajeAPlanificador->tipoProceso = mensajeDePlanif->tipoProceso;
	parametros->mensajeAPlanificador->tipoOperacion = mensajeDePlanif->tipoOperacion;
	parametros->mensajeAPlanificador->estado = mensajeDePlanif->estado;
	parametros->mensajeAPlanificador->pid = mensajeDePlanif->pid;
	parametros->mensajeAPlanificador->counterProgram = mensajeDePlanif->counterProgram;
	parametros->mensajeAPlanificador->quantum = mensajeDePlanif->quantum;
	parametros->mensajeAPlanificador->mensaje = malloc(strlen(mensajeDePlanif->mensaje)+1);
	//parametros->mensajeAPlanificador->mensaje = malloc(mensajeDePlanif->tamanioMensaje + 1);
    //memset(parametros->mensajeAPlanificador->mensaje,'\0',mensajeDePlanif->tamanioMensaje);
    //memcpy(parametros->mensajeAPlanificador->mensaje, mensajeDePlanif->mensaje,strlen (mensajeDePlanif->mensaje));
	strcpy(parametros->mensajeAPlanificador->mensaje, mensajeDePlanif->mensaje);
	parametros->mensajeAPlanificador->tamanioMensaje = strlen(parametros->mensajeAPlanificador->mensaje)+1;
	//parametros->mensajeAPlanificador->mensaje[mensajeDePlanif->tamanioMensaje] = '\0';

}

void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion) {
	char* logueoContexto = (char*)malloc(50);
	char* estado;
	if (contextoDeEjecucion->estado == LISTO) {
		estado = malloc(7);
		strcpy(estado, " LISTO");
	}
	if (contextoDeEjecucion->estado == IO) {
		estado = malloc(7);
		strcpy(estado, " IO");
	}
	if (contextoDeEjecucion->estado == EJECUTANDO) {
		estado = malloc(12);
		strcpy(estado, " EJECUTANDO");
	}
	if (contextoDeEjecucion->estado == FINALIZADO) {
		estado = malloc(12);
		strcpy(estado, " FINALIZADO");
	}

	strcpy(logueoContexto, "Contexto de ejecucion recibido: PID: ");
	string_append(&logueoContexto, string_itoa(contextoDeEjecucion->pid));
	string_append(&logueoContexto, "\nInstruccion: ");
	string_append(&logueoContexto,
			string_itoa(contextoDeEjecucion->counterProgram));
	string_append(&logueoContexto, " \nQuantum: ");
	string_append(&logueoContexto, string_itoa(contextoDeEjecucion->quantum));
	string_append(&logueoContexto, " \nEstado: ");
	string_append(&logueoContexto, estado);
	string_append(&logueoContexto, " \nRuta: ");
	string_append(&logueoContexto, contextoDeEjecucion->mensaje);

	log_info(logCpu, logueoContexto);
	free(estado);
    free(logueoContexto);

}

void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida){

	char* logueoMemoria = malloc(sizeof(char) * 10);

	strcpy(logueoMemoria, "mProc: ");
		string_append(&logueoMemoria, string_itoa(respuestaMemoria->pid));
		string_append(&logueoMemoria, " - ");

		if (respuestaMemoria->codOperacion == 'i') {
			string_append(&logueoMemoria, "Iniciado \n");
		}
		if (respuestaMemoria->codOperacion == 'l') {
			string_append(&logueoMemoria, "Pagina: ");
			string_append(&logueoMemoria, string_itoa(respuestaMemoria->numeroPagina));
			string_append(&logueoMemoria, " leida: ");
			string_append(&logueoMemoria, respuestaMemoria->mensaje);
		}
		//if(string_starts_with(message->lineaDeProceso,"escribir")) {  } //TODO cheackpoint 3 supongo
		//if(string_starts_with(message->lineaDeProceso,"entrada-salida")) { }
		if ((respuestaMemoria->codOperacion == 'f') && (respuestaMemoria->codAux == 'a')) {
			string_append(&logueoMemoria, "Fallo \n");
		}

		if ((respuestaMemoria->codOperacion == 'f') && (respuestaMemoria->codAux == 'i')) {
					string_append(&logueoMemoria, "Finalizado");
				}
		log_info(logCpu, logueoMemoria);
		free(logueoMemoria);

}

