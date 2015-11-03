/*
 * estructuras.h
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#ifndef SRC_ESTRUCTURAS_H_
#define SRC_ESTRUCTURAS_H_

#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>


sem_t hayProgramas;

t_queue* colaProcesos;
t_queue* colaIO;
t_list* listaEjecutando;
t_list* listaCpuLibres;


t_log* logPlanificador;

typedef struct{
	t_list* listaCpus;
	t_queue* procesos;
}tParametroEnviar;

typedef struct {
	int socket;
	t_list* listaCpus;
} tParametroSelector;
typedef struct {
	char* puertoEscucha;
	char algoritmo;
	int quantum;
}tconfig_planif;

typedef enum {
	LISTO, IO, EJECUTANDO, FINALIZADO
}testado;

typedef struct {
	char* ruta;
	int pid;
	char* nombre;
	testado estado;
	int siguiente;
	int maximo;
}tpcb;

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

#endif /* SRC_ESTRUCTURAS_H_ */
