/*
 * logueo.c
 *
 *  Created on: 3/11/2015
 *      Author: utnso
 */

#include "funcionesPlanificador.h"
#include <commons/string.h>
#include <commons/log.h>

/*void convertirOperacionAInstruccion(char operacion, char** instruccion){
	switch(operacion){

	case 'i': string_append(&instruccion, "Iniciado"); break;
	case 'e': string_append(&instruccion, "Entrada-Salida"); break;
	case 'l': string_append(&instruccion, "Lectura"); break;
	case 'E': string_append(&instruccion, "Escritura"); break;
	case 'f': string_append(&instruccion, "Finalizo"); break;

	}
}*/

void logueoProcesos(int pid, char* path, char operacion){//faltaria cuando finaliza, mas facil hacemos otra funcion xD

	char* logueo = (char*)malloc(40+strlen(path));
	if (operacion == 'i')
		log_info(logPlanificador,"Proceso %s con PID: %d Iniciado\n",nombrePrograma(path),pid);
	else
		log_info(logPlanificador,"Proceso %s con PID: %d Finalizado\n",nombrePrograma(path),pid);
	free(logueo);
}

void logueoConexionCPUS(int socket){
	char* logueo = (char*)malloc(sizeof(36));

	if(socket < 0) log_info(logPlanificador,"Conexion en socket %d cerrada\n",socket);

	else log_info(logPlanificador,"Nueva conexion en socket %d\n",socket);

	free(logueo);
}

char* contenidoDeColas(t_list* lista){
	int i;
	char* contenido = string_new();
	for(i = 0; i < list_size(lista); i++){
		tpcb* pcb = list_get(lista,i);
		string_append_with_format(&contenido,"PID: %d ",pcb->pid);
		string_append(&contenido,nombrePrograma(pcb->ruta));
		contenido[strlen(contenido)-1] = '\0';
		string_append(&contenido,",");
	}
	return contenido;
}

void logueoAlgoritmo(int inicial,char* mProc){
	char* logueo = (char*)malloc(65+strlen(mProc));
	char* algoritmo = string_new();
	if(inicial == 0) string_append(&algoritmo,"FIFO");
	else string_append(&algoritmo,"RR");
	log_info(logPlanificador,"El Proceso %s se encuentra ejecutando algoritmo %s\n",nombrePrograma(mProc),algoritmo);
	//Envio mensaje para loguear colas de planificacion
	char* contenido = string_new();
	pthread_mutex_lock(&mutexSwitchProc);

	string_append(&contenido,"\nCola Ready: ");
	string_append(&contenido,contenidoDeColas(colaListos->elements));
	string_append(&contenido,contenidoDeColas(listaInicializando));

	string_append(&contenido,"\nCola Ejecutando: ");
	string_append(&contenido,contenidoDeColas(listaEjecutando));

	string_append(&contenido,"\nCola I/O: ");
	string_append(&contenido,contenidoDeColas(colaIO->elements));

	pthread_mutex_unlock(&mutexSwitchProc);

	log_info(logPlanificador,contenido);
	free(contenido);
	free(algoritmo);
	free(logueo);
}

/*void logueoRafaga(char* mProc, char operacion){
	char* logueo = (char*)malloc(65+strlen(mProc));
	char* instruccion = string_new();
	string_append_with_format(&logueo,"Rafaga completada del proceso %s \n",nombrePrograma(mProc));
	convertirOperacionAInstruccion(operacion,&instruccion);
	string_append(&logueo,instruccion);
	log_info(logPlanificador,"Rafaga completada del proceso %s \n",nombrePrograma(mProc));
	//Ver si la cpu envia toda la rafaga en un mensaje y loguear eso

	free(logueo);
}*/
