/*
 * estructuras.h
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#ifndef SRC_ESTRUCTURAS_H_
#define SRC_ESTRUCTURAS_H_

#include <stdbool.h>
#include <commons/collections/list.h>

// tabla de paginas
typedef struct {
	int nro_pagina;
	int	nro_marco;
	bool fue_modificado;
	bool en_uso;
} pagina_direccion ;

typedef struct {
	int pid;
	t_list * list_pagina_direccion;
} tabla_paginas;


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
	char codAux;
	int pid;
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
