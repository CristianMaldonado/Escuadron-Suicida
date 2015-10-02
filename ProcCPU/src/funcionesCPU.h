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

void interpretarInstruccion(char* instruccion, tParametroHilo* mensajeParaArmar);
void* serializarPaqueteMemoria(protocolo_cpu_memoria* paquete, int* tamanio);
void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida);
void enviarAMemoria(tParametroHilo* message);
void armarPaquete(protocolo_cpu_memoria* paquete,char tipoProceso, char codOperacion, int pid, int nroPagina, char* mensaje);
void armarPaquetePlanificador(protocolo_planificador_cpu* paquete,char tipoProceso, char codOperacion, int pid, testado estado, int counterProgram, int quantum, int tamanioMensaje, char* mensaje);
char* leerInstruccion(int* instructionPointer, char* lineaLeida, FILE* archivo, int tam);
int deserializarMemoria(protocolo_memoria_cpu* package);
int deserializarPlanificador(protocolo_planificador_cpu* package);
void cargarParametrosHilo(protocolo_planificador_cpu* mensajeDePlanif,tParametroHilo* parametros);
void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion);
void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida);
void crearMockitoPlanif(protocolo_planificador_cpu* package);

#endif /* SRC_FUNCIONESCPU_H_ */
