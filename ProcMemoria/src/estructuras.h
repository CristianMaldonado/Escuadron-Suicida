/*
 * estructuras.h
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#ifndef SRC_ESTRUCTURAS_H_
#define SRC_ESTRUCTURAS_H_

typedef struct {
	char* puertoEscucha;
	char* ipSwap;
	char* puertoSwap;
	int maximoMarcosPorProceso;
	int cantidadMarcos;
	int tamanioMarco;
	int entradasTLB;
	char habilitadaTLB;
	int retardoMemoria;
}   tconfig_memoria;

// estructura para leer el protocolo desde el cpu a la memoria
typedef struct {
	char cod_op;
	int pid;
	int paginas;
	int tamanio_mensaje;
	char* mensaje;
}  __attribute__((packed))  tprotocolo_desde_cpu_y_hacia_swap;

typedef struct {
	int pid;
	char error; // 's'=hubo error,  'n'= no hubo error
	int tamanio;
	char *mensaje;
}   __attribute__((packed)) tprotocolo_swap_memoria;

typedef struct {
	char cod_proceso; // siempre una 'm'
	char cod_op;
	char cod_aux;
	int pid;
	int numero_pagina;
	int tamanio_mensaje;
	char *mensaje;
} __attribute__((packed))  tprotocolo_memoria_cpu;

#endif /* SRC_ESTRUCTURAS_H_ */
