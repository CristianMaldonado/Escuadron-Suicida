/*
 * funcionesPlanificador.h
 *
 *  Created on: 22/9/2015
 *      Author: utnso
 */

#ifndef SRC_FUNCIONESPLANIFICADOR_H_
#define SRC_FUNCIONESPLANIFICADOR_H_

tpcb armarPCB (char* path,int cant);

int clasificarComando(char* message);

void procesarComando(int nro_comando, char* message, int cantProc,t_queue* colaProc,sem_t* sem);

/*char* definirMensaje(tpcb* pcb);*/

#endif /* SRC_FUNCIONESPLANIFICADOR_H_ */
