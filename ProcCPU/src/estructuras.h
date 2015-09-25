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
	char tipoMensaje;
	enum estado;
	int pid;
	int siguiente;
	int quantum;
	int tamanioMensaje;
	char* mensaje;
} protocolo_planificador_cpu;                            //TODO: REVISARRR

typedef struct {
	char codOperacion;
	char codAux;
	int pid;
	int nroPagina;
	int tamanio; //Agregado el tamaño del mensaje
	char* mensaje;
} protocolo_cpu_memoria;

typedef struct{
	int socketMemoria;
	int socketPlanificador;
	char* lineaDeProceso;
	protocolo_cpu_memoria paquete;
}tMensajeAMemoria;

#endif /* SRC_ESTRUCTURAS_H_ */
