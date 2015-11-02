/*
 * logueo.c
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */

#include "logueo.h"
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>

void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion,int tid) {//eliminar tid
	char* logueoContexto = (char*)malloc(50);
	char* estado;
	if (contextoDeEjecucion->estado == LISTO) {
		estado = malloc(7);
		strcpy(estado, "LISTO");
	}
	if (contextoDeEjecucion->estado == IO) {
		estado = malloc(7);
		strcpy(estado, "IO");
	}
	if (contextoDeEjecucion->estado == EJECUTANDO) {
		estado = malloc(12);
		strcpy(estado, "EJECUTANDO");
	}
	if (contextoDeEjecucion->estado == FINALIZADO) {
		estado = malloc(12);
		strcpy(estado, "FINALIZADO");
	}

	strcpy(logueoContexto, "Contexto de ejecucion recibido: \nPID: ");
	string_append_with_format(&logueoContexto, "%d", contextoDeEjecucion->pid);
	string_append(&logueoContexto, "\nInstruccion: ");
	string_append_with_format(&logueoContexto, "%d", contextoDeEjecucion->counterProgram);
	string_append(&logueoContexto, " Quantum: ");
	string_append_with_format(&logueoContexto, "%d", contextoDeEjecucion->quantum);
	string_append(&logueoContexto, " Estado: ");
	string_append(&logueoContexto, estado);
	string_append(&logueoContexto, " \nRuta: ");
	string_append(&logueoContexto, contextoDeEjecucion->mensaje);
	string_append_with_format(&logueoContexto, " Recibido por el CPU %d\n", tid);

	log_info(logCpu, logueoContexto);
	free(estado);
    free(logueoContexto);

}

char* prepararLogueoDeMemoria(protocolo_memoria_cpu* respuestaMemoria){

	char* texto = malloc(sizeof(char) * 15);

	//string_append(&(*texto),"\nmProc: ");
	strcpy(texto,"\nmProc: ");
	string_append_with_format(&texto, "%d", respuestaMemoria->pid);
	string_append(&texto, " - ");

	if (respuestaMemoria->codOperacion == 'i' && respuestaMemoria->codAux != 'a') {
		string_append(&texto, "Iniciado");
	}
	if (respuestaMemoria->codOperacion == 'l') {
		string_append(&texto, "Pagina: ");
		string_append_with_format(&texto, "%d", respuestaMemoria->numeroPagina);
		string_append(&texto, " leida: ");
		string_append(&texto, respuestaMemoria->mensaje);
	}

	if (respuestaMemoria->codOperacion == 'e'){
		string_append(&texto, "Pagina: ");
		string_append_with_format(&texto, "%d", respuestaMemoria->numeroPagina);
		string_append(&texto, " escrita: ");
		string_append(&texto, respuestaMemoria->mensaje);
	}

	if ((respuestaMemoria->codOperacion == 'i') && (respuestaMemoria->codAux == 'a')) {
		string_append(&texto, "Fallo al iniciar\n");
	}

	if ((respuestaMemoria->codOperacion == 'f') && (respuestaMemoria->codAux == 'f')) {
		string_append(&texto, "Finalizado");
	}
	return texto;
}

/*
void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria){

	char* logueoMemoria = malloc(sizeof(char) * 10);

	strcpy(logueoMemoria, "mProc: ");
	string_append_with_format(&logueoMemoria, "%d", respuestaMemoria->pid);
	string_append(&logueoMemoria, " - ");

	if (respuestaMemoria->codOperacion == 'i' && respuestaMemoria->codAux != 'a') {
		string_append(&logueoMemoria, "Iniciado");
	}
	if (respuestaMemoria->codOperacion == 'l') {
		string_append(&logueoMemoria, "Pagina: ");
		string_append_with_format(&logueoMemoria, "%d", respuestaMemoria->numeroPagina);
		string_append(&logueoMemoria, " leida: ");
		string_append(&logueoMemoria, respuestaMemoria->mensaje);
	}

	if (respuestaMemoria->codOperacion == 'e'){
		string_append(&logueoMemoria, "Pagina: ");
		string_append_with_format(&logueoMemoria, "%d", respuestaMemoria->numeroPagina);
		string_append(&logueoMemoria, " escrita: ");
		string_append(&logueoMemoria, respuestaMemoria->mensaje);
	}

	if ((respuestaMemoria->codOperacion == 'i') && (respuestaMemoria->codAux == 'a')) {
		string_append(&logueoMemoria, "Fallo al iniciar\n");
	}

	if ((respuestaMemoria->codOperacion == 'f') && (respuestaMemoria->codAux == 'f')) {
		string_append(&logueoMemoria, "Finalizado");
	}
	log_info(logCpu, logueoMemoria);
	free(logueoMemoria);

}*/

void loguearPlanificadorIO(protocolo_planificador_cpu* mensajeDePlanificador, int tiempo){
	char* logueoIO = malloc(sizeof(char) * 10);
	strcpy(logueoIO, "mProc: ");
	string_append_with_format(&logueoIO, "%d", mensajeDePlanificador->pid);
	string_append(&logueoIO, " en entrada-salida de tiempo ");
	string_append_with_format(&logueoIO, "%d", tiempo);

	log_info(logCpu, logueoIO);
	free(logueoIO);
}
