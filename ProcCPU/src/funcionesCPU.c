/*
 * funcionesCPU.c
 *
 *  Created on: 23/9/2015
 *      Author: utnso
 */

#include "funcionesCPU.h"
#include "serializacion.h"
#include <commons/error.h>
#include "estructuras.h"

void interpretarInstruccion(tMensajeAMemoria* message){

	if(string_starts_with(message->lineaDeProceso,"iniciar")){ pedirMemoria(message); }
	if(string_starts_with(message->lineaDeProceso,"leer")) { leerPagina(message); }
	if(string_starts_with(message->lineaDeProceso,"escribir")) { escribirTexto(message); }
	if(string_starts_with(message->lineaDeProceso,"entrada-salida")) { tenesQueEsperar(message); }
	if(string_starts_with(message->lineaDeProceso,"finalizar")) { processKill(message); }
}

void pedirMemoria(tMensajeAMemoria* message){
	//armarPaquete();
	//serializar();
	//send(message.socket);
	//sem_post(&ejecutaInstruccion);
}

void leerPagina(tMensajeAMemoria* message){
	//armarPaquete();
	//serializar();
	//send(message.socket);
	//sem_post(&ejecutaInstruccion);
}

void escribirTexto(tMensajeAMemoria* message){

}

void tenesQueEsperar(tMensajeAMemoria* message){

}

void processKill(tMensajeAMemoria* message){

}

void armarPaquete(protocolo_cpu_memoria* aux, char codOperacion, char codAux,//tendria que ir en serializacion
		int pid, int nroPagina, char* mensaje) {

	aux->codOperacion = codOperacion;
	aux->codAux = codAux;
	aux->pid = pid;
	aux->nroPagina = nroPagina;
	strcpy(aux->mensaje, mensaje);

	//TODO Hacerlo mas genérico con un booleano y cargue la estructura (sin mandar todos los parametros)
}
