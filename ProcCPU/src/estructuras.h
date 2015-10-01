/*
 * estructuras.h
 *
 *  Created on: 11/9/2015
 *      Author: utnso
 */

#ifndef SRC_ESTRUCTURAS_H_
#define SRC_ESTRUCTURAS_H_

#include <semaphore.h>
#include <commons/log.h>
sem_t ejecutaInstruccion;
t_log *logCpu;
int socketPlanificador;
int socketMemoria;

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
	int quantum;           //SI ES 0 EL ALGORITMO ES FIFO, SINO RR
	int tamanioMensaje;
	char* mensaje;//en este caso es la ruta del "mprod"
}__attribute__((packed)) protocolo_planificador_cpu;


typedef struct {
	char tipoProceso;
	char tipoOperacion;
	//char codAux;
	int pid;
	int nroPagina;
	int tamanioMensaje;
	char* mensaje;
}__attribute__((packed)) protocolo_cpu_memoria;

typedef struct{
	char tipoProceso;
	char codOperacion;
	char codAux;
	int pid;
	int numeroPagina;
	int tamanioMensaje;
	char *mensaje;

}__attribute__((packed)) protocolo_memoria_cpu;

typedef struct{
	protocolo_cpu_memoria* mensajeAMemoria;
	protocolo_planificador_cpu* mensajeAPlanificador;
	protocolo_memoria_cpu* mensajeDeMemoria;

}tParametroHilo;

#endif /* SRC_ESTRUCTURAS_H_ */
