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
/*
void interpretarInstruccion(tMensajeAMemoria* message){

	if(string_starts_with(message->lineaDeProceso,"iniciar")){ enviar(message); }
	if(string_starts_with(message->lineaDeProceso,"leer")) { enviar(message); }
	//if(string_starts_with(message->lineaDeProceso,"escribir")) { enviar(message); }
	//if(string_starts_with(message->lineaDeProceso,"entrada-salida")) { enviar(message); }
	if(string_starts_with(message->lineaDeProceso,"finalizar")) { enviar(message); }
}

void enviar(tMensajeAMemoria* message){
	protocolo_cpu_memoria paquete = armarPaquete(message->lineaDeProceso);
	protocolo_cpu_memoria empaquetado = serializar(paquete); //TODO : REVISAR ARMAR PAQUETE (PARAM A ENVIAR)
	send(message->socketMemoria, empaquetado, sizeof(empaquetado));
	sem_post(&ejecutaInstruccion);
}*/

//MODIFICAR ARMAR PAQUETE PARAMETROS
void armarPaquete(protocolo_cpu_memoria* aux, char codOperacion, char codAux,//tendria que ir en serializacion
		int pid, int nroPagina, char* mensaje) {

	aux->codOperacion = codOperacion;
	aux->codAux = codAux;
	aux->pid = pid;
	aux->nroPagina = nroPagina;
	strcpy(aux->mensaje, mensaje);

	//TODO Hacerlo mas genérico con un booleano y cargue la estructura (sin mandar todos los parametros)
}
