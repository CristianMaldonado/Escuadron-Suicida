/*
 * funcionesCPU.c
 *
 *  Created on: 23/9/2015
 *      Author: utnso
 */

#include <commons/error.h>
#include "estructuras.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "logueo.h"
#include "serializacion.h"
#include <pthread.h>

void enviarAMemoria(protocolo_cpu_memoria* message) {
	int tamanio;
	void* empaquetado = serializarPaqueteMemoria(message,&tamanio);
	send(socketMemoria, empaquetado, tamanio,0);
	free(empaquetado);
}

void armarPaquetePlanificador(protocolo_planificador_cpu* paquete, char tipoProceso,char codOperacion, int pid, testado estado, int counterProgram ,int quantum, int tamanioMensaje,char* mensaje) {
	paquete->tipoProceso = tipoProceso;
	paquete->tipoOperacion = codOperacion;
	paquete->pid = pid;
	paquete->estado = estado;
	paquete->counterProgram = counterProgram;
	paquete->quantum = quantum;
	paquete->tamanioMensaje = strlen(mensaje) + 1;
	paquete->mensaje = malloc(paquete->tamanioMensaje);
	strcpy(paquete->mensaje, mensaje);
}

void actualizarOperacionPaquetePlanificador(protocolo_planificador_cpu* paquete, char tipoOperacion){
	paquete->tipoOperacion = tipoOperacion;
	//TODO: Modificar estado (?
}

void actualizarOperacionPaquetePlanificadorIO(protocolo_planificador_cpu* paquete, char tipoOperacion,int IO){
	paquete->tipoOperacion = tipoOperacion;
	paquete->mensaje = malloc(4);
	strcpy(paquete->mensaje,string_itoa(IO));
}

void enviarAPlanificador(protocolo_planificador_cpu* respuestaDeMemo,int socketPlanificador){

	int tamanio;
	void* empaquetado = serializarPaquetePlanificador(respuestaDeMemo,&tamanio);
	send(socketPlanificador, empaquetado, tamanio, 0);
	free(empaquetado);
}
//MODIFICAR ARMAR PAQUETE PARAMETROS
void armarPaqueteMemoria(protocolo_cpu_memoria* paquete,char codOperacion, int pid, int nroPagina, char* mensaje) {
	paquete->tipoOperacion = codOperacion;
	paquete->pid = pid;
	paquete->nroPagina = nroPagina;
	paquete->tamanioMensaje = strlen(mensaje) + 1;
	paquete->mensaje = malloc(paquete->tamanioMensaje);
	strcpy(paquete->mensaje, mensaje);
}

void interpretarInstruccion(char* instruccion, protocolo_planificador_cpu* mensajeDePlanificador,protocolo_cpu_memoria* mensajeParaArmar,int socketPlanificador) {

		char** linea = string_split(instruccion, ";");
		char** lineaFiltrada = string_split(linea[0]," ");

		if (string_starts_with(instruccion, "iniciar")) {
			int numero = atoi(lineaFiltrada[1]);
			armarPaqueteMemoria(mensajeParaArmar, 'i',mensajeDePlanificador->pid,numero , "-");
		}
		if (string_starts_with(instruccion, "leer")) {
			int numero = atoi(lineaFiltrada[1]);
			armarPaqueteMemoria(mensajeParaArmar, 'l',mensajeDePlanificador->pid, numero, "-");
		}
		if(string_starts_with(instruccion,"escribir")) {
			int numero = atoi(lineaFiltrada[1]);
			armarPaqueteMemoria(mensajeParaArmar, 'e',mensajeDePlanificador->pid, numero, lineaFiltrada[2]);
		}

		if(string_starts_with(instruccion,"entrada-salida")) {
			int tiempo = atoi(lineaFiltrada[1]);
			actualizarOperacionPaquetePlanificadorIO(mensajeDePlanificador,'e',tiempo);
            enviarAPlanificador(mensajeDePlanificador,socketPlanificador);
            loguearPlanificadorIO(mensajeDePlanificador, tiempo);
		}

		if (string_starts_with(instruccion, "finalizar;")) {
				armarPaqueteMemoria(mensajeParaArmar, 'f', mensajeDePlanificador->pid, 0, "-");
		}
		free(linea);
		free(lineaFiltrada);
}

/*Lee del archivo la linea indicada por el Instruction Pointer*/
char* leerInstruccion(int* instructionPointer,char* lineaLeida, FILE* archivo, int tam) {
	fseek(archivo,0,SEEK_SET);
	int cont = 1;

	while (!feof(archivo) && cont <= (*instructionPointer) ) {
		fgets(lineaLeida, tam, archivo);
		cont++;
	}

	(*instructionPointer) = (*instructionPointer) + 1;

	if (!string_starts_with(lineaLeida, "finalizar;")) lineaLeida[strlen(lineaLeida)-1] = '\0';

	return lineaLeida;

}


