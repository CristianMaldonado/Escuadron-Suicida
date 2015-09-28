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
char* serializarPaqueteMemoria(protocolo_cpu_memoria* paquete);
void enviar(tParametroHilo* message);
void armarPaquete(protocolo_cpu_memoria* paquete,char tipoProceso, char codOperacion, int pid, int nroPagina, char* mensaje);
void liberar_paquete(char **paquete);
char* leerInstruccion(int* instructionPointer, FILE* archivo);
int recivir_deserializar(protocolo_planificador_cpu *package, int socketPlanificador);
void cargarParametrosHilo(int socketPlanificador,int socketMemoria,protocolo_planificador_cpu* mensajeDePlanif,tParametroHilo* parametros);
void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion);

#endif /* SRC_FUNCIONESCPU_H_ */
