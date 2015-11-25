/*
 * logueo.h
 *
 *  Created on: 3/11/2015
 *      Author: utnso
 */

#ifndef SRC_LOGUEO_H_
#define SRC_LOGUEO_H_


#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include "estructuras.h"

void logueoProcesos(int pid, char* path, char operacion);
void logueoConexionCPUS(int socket);
void logueoAlgoritmo(int algoritmo,char* mProc);
/*

void logueoRafaga();

void loguearColas(t_list* lista);
void loguearColaIO(t_list* lista);
void logueoAnteultimo(char* mProc, int inicial);

*/
void logueoFinalizado(tpcb* pcb);


#endif /* SRC_LOGUEO_H_ */

