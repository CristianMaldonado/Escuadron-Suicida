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
#include "logueo.h"
#include "serializacion.h"
#include <pthread.h>

/*void crearMockitoPlanif(protocolo_planificador_cpu* package){

	package->tipoProceso = 'P';
	package->tipoOperacion = 'l';
	package->estado = EJECUTANDO;
	package->pid = 1;
	package->counterProgram = 2;
	package->quantum = 0;
	package->mensaje = malloc(strlen("programa.txt")+1);
	strcpy(package->mensaje,"programa.txt");
	package->tamanioMensaje = strlen(package->mensaje)+1;

}*/

void enviarAMemoria(protocolo_cpu_memoria* message) {
	int tamanio;
	void* empaquetado = serializarPaqueteMemoria(message,&tamanio);
	pthread_mutex_lock(&mutex);
	send(socketMemoria, empaquetado, tamanio,0);
	pthread_mutex_unlock(&mutex);
	free(empaquetado); //free(1)
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

void enviarAPlanificador(protocolo_planificador_cpu* respuestaDeMemo,int socketPlanificador){

	int tamanio;
	void* empaquetado = serializarPaquetePlanificador(respuestaDeMemo,&tamanio);
	int aa = send(socketPlanificador, empaquetado, tamanio, 0);
	printf("send enviar plani: %d\n", aa);
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
		} //TODO cheackpoint 3 supongo

		if(string_starts_with(instruccion,"entrada-salida")) {
			int tiempo = atoi(lineaFiltrada[1]);
			//armarPaquetePlanificador(mensajeDePlanificador, 'c','E', mensajeDePlanificador->pid, mensajeDePlanificador->estado,
			//		mensajeDePlanificador->counterProgram,mensajeDePlanificador->quantum, mensajeDePlanificador->tamanioMensaje, mensajeDePlanificador->mensaje);
            actualizarOperacionPaquetePlanificador(mensajeDePlanificador,'e');
            enviarAPlanificador(mensajeDePlanificador,socketPlanificador);
            loguearPlanificadorIO(mensajeDePlanificador, tiempo);
		}

		if (string_starts_with(instruccion, "finalizar;")) {
				armarPaqueteMemoria(mensajeParaArmar, 'f', mensajeDePlanificador->pid, 0, "-");
				//armarPaquetePlanificador(mensajeDePlanificador, 'c','f', mensajeDePlanificador->pid, mensajeDePlanificador->estado,
				//mensajeDePlanificador->counterProgram,mensajeDePlanificador->quantum, mensajeDePlanificador->tamanioMensaje, mensajeDePlanificador->mensaje);
				//enviarAPlanificador(mensajeDePlanificador,socketPlanificador);

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

/*void calcularTamanioDeLinea(FILE* archivo,int* tamanio){
	int i;

	for(i=0;(getc(archivo) != '\n') && (!feof(archivo));i++);

	tamanio = &i;
}*/


