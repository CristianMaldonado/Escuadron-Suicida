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

void liberar_paquete(char **paquete){
	free(*paquete);
}

char* serializarPaquete(protocolo_cpu_memoria* paquete) {//malloc(1)
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

int recivir_deserializar(protocolo_planificador_cpu *package, int socketPlanificador){//TODO deserializar mensaje de planificador
	int status;/*
	int buffer_size;
	char *buffer = malloc(buffer_size = sizeof(uint32_t));

	uint32_t username_long;

	status = recv(socketPlanificador, buffer, sizeof(package->tipoProceso), 0);

	memcpy(&(username_long), buffer, buffer_size);

	if (!status) return 0;
	status = recv(socketPlanificador, package->tipoOperacion, username_long, 0);
    if (!status) return 0;

    uint32_t message_long;

    status = recv(socketPlanificador, buffer, sizeof(package->tamanioMensaje), 0);

    memcpy(&(message_long), buffer, buffer_size);

    if (!status) return 0;

    status = recv(socketPlanificador, package->mensaje, message_long, 0);

    if (!status) return 0;
    free(buffer);*/
    return status;
}

void enviar(tParametroHilo* message) {
	char* empaquetado = serializarPaquete(message->mensajeAMemoria);
	send(message->socketMemoria, empaquetado, string_length(empaquetado)+1, 0);
	liberar_paquete(&empaquetado);//free(1)
}

void interpretarInstruccion(char* instruccion, tParametroHilo* mensajeParaArmar) {

	char** linea = string_split(instruccion, " ");
	if (string_starts_with(instruccion, "iniciar")) {
		armarPaquete(mensajeParaArmar->mensajeAMemoria, 'c', 'i', mensajeParaArmar->mensajeAPlanificador->pid, atoi(linea[1]),"\0");
	}
	if (string_starts_with(instruccion, "leer")) {
		armarPaquete(mensajeParaArmar->mensajeAMemoria, 'c', 'l', mensajeParaArmar->mensajeAPlanificador->pid, atoi(linea[1]),"\0");
	}
	//if(string_starts_with(message->lineaDeProceso,"escribir")) {  } //TODO cheackpoint 3 supongo
	//if(string_starts_with(message->lineaDeProceso,"entrada-salida")) { }
	if (string_starts_with(instruccion, "finalizar")) {
		armarPaquete(mensajeParaArmar->mensajeAMemoria, 'c', 'f', mensajeParaArmar->mensajeAPlanificador->pid, 0,"\0");
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

char* leerMprod(char* rutaDelMprod, int instructionPointer){//ruta+instruction pointer => leo la linea del ip y la devuelvo
	char* lineaLeida;//TODO pedir memoria,
	FILE* archivo = fopen(rutaDelMprod,"r");
	fseek(archivo,instructionPointer,SEEK_CUR);
	while(!feof(archivo)){

		//buscar alguna funcion o forma de leer una linea hasta \n (ciclo for?) y usar strcpy
		//usar esto o podria haber una funcion abrir archivo, leer linea, etc
	}

	fclose(archivo);
	return lineaLeida;
}

void cargarParametrosHilo(int socketPlanificador,int socketMemoria,protocolo_planificador_cpu* mensajeDePlanif,tParametroHilo* parametros){

	parametros->socketMemoria = socketMemoria;
	parametros->socketPlanificador = socketPlanificador;
	parametros->mensajeAPlanificador->tipoProceso = mensajeDePlanif->tipoProceso;
	parametros->mensajeAPlanificador->tipoOperacion = mensajeDePlanif->tipoOperacion;
	parametros->mensajeAPlanificador->estado = mensajeDePlanif->estado;
	parametros->mensajeAPlanificador->counterProgram = mensajeDePlanif->counterProgram;
	parametros->mensajeAPlanificador->quantum = mensajeDePlanif->quantum;
	parametros->mensajeAPlanificador->tamanioMensaje = mensajeDePlanif->tamanioMensaje;
	strcpy(parametros->mensajeAPlanificador->mensaje,mensajeDePlanif->mensaje);
}


void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion){
	char* logueoContexto=malloc(37);
	char* estado=malloc(sizeof(testado));
	if(contextoDeEjecucion->estado == LISTO){strcpy(estado," LISTO");}
	if(contextoDeEjecucion->estado == IO){strcpy(estado," IO");}
	if(contextoDeEjecucion->estado == EJECUTANDO){strcpy(estado," EJECUTANDO");}
	if(contextoDeEjecucion->estado == FINALIZADO){strcpy(estado," FINALIZADO");}

	strcpy(logueoContexto,"Contexto de ejecucion recibido: PID: ");
	string_append(&logueoContexto,string_itoa(contextoDeEjecucion->pid));
	string_append(&logueoContexto," Instruccion: ");
	string_append(&logueoContexto,string_itoa(contextoDeEjecucion->counterProgram));
	string_append(&logueoContexto," Quantum: ");
	string_append(&logueoContexto,string_itoa(contextoDeEjecucion->quantum));
	string_append(&logueoContexto," Estado: ");
	string_append(&logueoContexto,estado);
	string_append(&logueoContexto," Ruta: ");
	string_append(&logueoContexto,contextoDeEjecucion->mensaje);

	log_info(logCpu,logueoContexto);
	free(estado);
	free(logueoContexto);

}


















