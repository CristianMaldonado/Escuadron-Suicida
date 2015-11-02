/*
 * logueo.h
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */

#ifndef SRC_LOGUEO_H_
#define SRC_LOGUEO_H_

#include <commons/string.h>
#include "estructuras.h"

void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion,int tid);
//void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida);
void loguearPlanificadorIO(protocolo_planificador_cpu* mensajeDePlanificador, int tiempo);
char* prepararLogueoDeMemoria(protocolo_memoria_cpu* respuestaMemoria);
void loguearRafaga(char* textoALoguear);

#endif
