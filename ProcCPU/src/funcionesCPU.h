/*
 * funcionesCPU.h
 *
 *  Created on: 23/9/2015
 *      Author: utnso
 */

#ifndef SRC_FUNCIONESCPU_H_
#define SRC_FUNCIONESCPU_H_

#include <commons/string.h>
#include <stdio.h>
#include "estructuras.h"

void interpretarInstruccion(tMensajeAMemoria* message);
void armarPaquete(protocolo_cpu_memoria* aux, char codOperacion, char codAux,int pid, int nroPagina, char* mensaje);
void processKill(tMensajeAMemoria* message);
void tenesQueEsperar(tMensajeAMemoria* message);
void leerTexto(tMensajeAMemoria* message);
void pedirMemoria(tMensajeAMemoria* message);
void leerPagina(tMensajeAMemoria* message);

#endif /* SRC_FUNCIONESCPU_H_ */
