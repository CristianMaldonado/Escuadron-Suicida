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
} tconfig_memoria;

// estructura para leer el protocolo desde el cpu a la memoria
typedef struct {
	char cod_op;
	int pid;
	int paginas;
	int tamanio_mensaje;
	char* mensaje;
} tprotocolo;

#endif /* SRC_ESTRUCTURAS_H_ */
