/*
 * estructuras.h
 *
 *  Created on: 11/9/2015
 *      Author: utnso
 */

#ifndef SRC_ESTRUCTURAS_H_
#define SRC_ESTRUCTURAS_H_

#include <semaphore.h>
sem_t ejecutaInstruccion;

typedef enum {
	LISTO, IO, EJECUTANDO, FINALIZADO
}testado;

typedef struct {
	char* ipPlanificador;
	char* puertoPlanificador;
	char* ipMemoria;
	char* puertoMemoria;
	int cantidadHilos;
	int retardo;
} tipoConfiguracionCPU;


typedef struct {
	char tipoProceso;
	char tipoOperacion;
	testado estado;
	int pid;
	int counterProgram;
	int quantum;
	int tamanioMensaje;
	char* mensaje;
} protocolo_planificador_cpu;


typedef struct {
	char tipoProceso;
	char tipoOperacion;
	//char codAux;
	int pid;
	int nroPagina;
	int tamanioMensaje;
	char* mensaje;
} protocolo_cpu_memoria;

typedef struct{
	int socketMemoria;
	char* lineaDeProceso;
	int pid;
	protocolo_cpu_memoria* paquete;
}tMensajeAMemoria;

typedef struct{
	int socketPlanificador;
    //protocolo cpu a planif
}tMensajeAPlanificador;

#endif /* SRC_ESTRUCTURAS_H_ */
