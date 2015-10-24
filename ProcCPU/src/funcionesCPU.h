/*
 * funcionesCPU.h
 *
 *  Created on: 23/9/2015
 *      Author: utnso
 */

#ifndef SRC_FUNCIONESCPU_H_
#define SRC_FUNCIONESCPU_H_

#include <commons/string.h>
#include <stdio.h>
#include "estructuras.h"

void interpretarInstruccion(char* instruccion, protocolo_planificador_cpu* mensajeDePlanificador,protocolo_cpu_memoria* mensajeParaArmar);
void* serializarPaqueteMemoria(protocolo_cpu_memoria* paquete, int* tamanio);
void* serializarPaquetePlanificador(protocolo_planificador_cpu* paquete, int* tamanio);
void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida);
void enviarAMemoria(protocolo_cpu_memoria* message);
void armarPaqueteMemoria(protocolo_cpu_memoria* paquete,char tipoProceso, char codOperacion, int pid, int nroPagina, char* mensaje);
void armarPaquetePlanificador(protocolo_planificador_cpu* paquete, char tipoProceso,char codOperacion, int pid, testado estado, int counterProgram ,int quantum, int tamanioMensaje,char* mensaje);
void armarPaquetePlanificadorIO(protocolo_planificador_cpu* paquete);
char* leerInstruccion(int* instructionPointer, char* lineaLeida, FILE* archivo, int tam);
int deserializarMemoria(protocolo_memoria_cpu* package);
int deserializarPlanificador(protocolo_planificador_cpu* package);
void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion);
void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida);
void enviarAPlanificador(protocolo_planificador_cpu* respuestaDeMemo);
void crearMockitoPlanif(protocolo_planificador_cpu* package);


#endif /* SRC_FUNCIONESCPU_H_ */
