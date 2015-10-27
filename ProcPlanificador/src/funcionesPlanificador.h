/*
 * funcionesPlanificador.h
 *
 *  Created on: 22/9/2015
 *      Author: utnso
 */

#ifndef SRC_FUNCIONESPLANIFICADOR_H_
#define SRC_FUNCIONESPLANIFICADOR_H_

#include "estructuras.h"


tpcb* armarPCB (char* path,int cant);

int clasificarComando(char* message);
void adaptadorPCBaProtocolo(tpcb* pcb,protocolo_planificador_cpu* paquete);
void procesarComando(int nro_comando, char* message, int cantProc,t_queue* colaProc);

int deserializarCPU(protocolo_planificador_cpu *package,int socketCPU);

void* serializarPaqueteCPU(protocolo_planificador_cpu* paquete, int* tamanio);

int maxLineas(FILE* archivo);

/*char* definirMensaje(tpcb* pcb);*/

#endif /* SRC_FUNCIONESPLANIFICADOR_H_ */
