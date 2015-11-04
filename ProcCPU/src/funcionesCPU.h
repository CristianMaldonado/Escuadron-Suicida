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

void interpretarInstruccion(char* instruccion, protocolo_planificador_cpu* mensajeDePlanificador,protocolo_cpu_memoria* mensajeParaArmar,int socketPlanificador);
char* leerInstruccion(int* instructionPointer, char* lineaLeida, FILE* archivo, int tam);
//void calcularTamanioDeLinea(FILE* archivo,int* tamanio);
void enviarAMemoria(protocolo_cpu_memoria* message);
void armarPaqueteMemoria(protocolo_cpu_memoria* paquete, char codOperacion, int pid, int nroPagina, char* mensaje);
void armarPaquetePlanificador(protocolo_planificador_cpu* paquete, char tipoProceso,char codOperacion, int pid, testado estado, int counterProgram ,int quantum, int tamanioMensaje,char* mensaje);
void actualizarOperacionPaquetePlanificador(protocolo_planificador_cpu* paquete, char tipoOperacion);
void enviarAPlanificador(protocolo_planificador_cpu* respuestaDeMemo,int socketPlanificador);
//void crearMockitoPlanif(protocolo_planificador_cpu* package);


#endif /* SRC_FUNCIONESCPU_H_ */
