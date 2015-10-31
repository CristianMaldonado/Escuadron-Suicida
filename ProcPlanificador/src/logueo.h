/*
 * logueo.h
 *
 *  Created on: 31/10/2015
 *      Author: utnso
 */


#ifndef SRC_LOGUEO_H_
#define SRC_LOGUEO_H_

#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include "estructuras.h"

void logueoProcesos(int pid, char* nombre);
void logueoConeccionCPUS(int socket,int conectado);
void logueoAlgoritmo(char algoritmo,char* mProc);
void logueoRafaga();

#endif /* SRC_LOGUEO_H_ */
