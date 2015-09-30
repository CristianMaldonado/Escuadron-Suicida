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
void* serializarPaqueteMemoria(protocolo_cpu_memoria* paquete);
void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida);
void enviar(tParametroHilo* message);
void armarPaquete(protocolo_cpu_memoria* paquete,char tipoProceso, char codOperacion, int pid, int nroPagina, char* mensaje);
void liberar_paquete(char **paquete);
char* leerInstruccion(int* instructionPointer, char* lineaLeida, FILE* archivo);
int deserializarMemoria(protocolo_memoria_cpu* package,int socketMemoria);
int deserializarPlanificador(protocolo_planificador_cpu* package,int socketPlanificador);
void cargarParametrosHilo(int socketPlanificador,int socketMemoria,protocolo_planificador_cpu* mensajeDePlanif,tParametroHilo* parametros);
void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion);
void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida);
void crearMockitoPlanif(protocolo_planificador_cpu* package);

#endif /* SRC_FUNCIONESCPU_H_ */
