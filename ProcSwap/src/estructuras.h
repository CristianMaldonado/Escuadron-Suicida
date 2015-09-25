/*
 * estructuras.h
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#ifndef SRC_ESTRUCTURAS_H_
#define SRC_ESTRUCTURAS_H_

// estructura para leer el archivo de configuracion
typedef struct {
	char* puertoEscucha;
	char* nombreSwap;
	int cantidadPaginas;
	int tamanioPagina;
	int retardo;
}  __attribute__((packed)) tconfig_swap;

typedef struct {
	int pid;
	char codigo_op;
	int cantidad_pagina;
	int tamanio_mensaje;
	char *mensaje;
}  __attribute__((packed)) tprotocolo_memoria_swap;

typedef struct {
	int pid;
	char error;
	int tamanio;
	char *mensaje;
}  __attribute__((packed)) tprotocolo_swap_memoria;

typedef struct{
	int pid;
	int comienzo;
	int paginas_ocupadas;
} tlista_ocupado;

typedef struct{
	int comienzo;
	int paginas_ocupadas;
} tlista_vacio;

typedef struct{
	int pid;
	int tamanio;
	char *buffer;
} tdatos_paginas;

#endif /* SRC_ESTRUCTURAS_H_ */
