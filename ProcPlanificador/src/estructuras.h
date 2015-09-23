/*
 * estructuras.h
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#ifndef SRC_ESTRUCTURAS_H_
#define SRC_ESTRUCTURAS_H_
#include <commons/collections/queue.h>


typedef struct{
	int socket;
	t_queue* procesos;
}tParametroEnviar;

typedef struct {
	char* puertoEscucha;
	char algoritmo;
	int quantum;
}tconfig_planif;

typedef enum {
	LISTO, IO, EJECUTANDO, FINALIZADO}
testado;

typedef struct {
	char* ruta;
	int pid;
	char* nombre;
	testado estado;
	int siguiente;
}tpcb;

#endif /* SRC_ESTRUCTURAS_H_ */
