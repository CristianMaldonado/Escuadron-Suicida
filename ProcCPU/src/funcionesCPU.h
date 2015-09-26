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
char* serializarPaquete(protocolo_cpu_memoria* paquete);
void enviar(tMensajeAMemoria* message);
void armarPaquete(protocolo_cpu_memoria* paquete,char tipoProceso, char codOperacion, int pid, int nroPagina, char* mensaje);
void liberar_paquete(char **paquete);

#endif /* SRC_FUNCIONESCPU_H_ */
