/*
 * logueo.c
 *
 *  Created on: 3/11/2015
 *      Author: utnso
 */


#include "logueo.h"
#include "funcionesPlanificador.h"

void logueoProcesos(int pid, char* nombre){//faltaria cuando finaliza, mas facil hacemos otra funcion xD
	nombre[strlen(nombre)-1] = '\0';
	char* logueo = (char*)malloc(40+strlen(nombre));
	log_info(logPlanificador,"Proceso %s con PID: %d Iniciado\n",nombrePrograma(nombre),pid);
	free(logueo);
}

void logueoConexionCPUS(int socket,int conectado){
	char* logueo = (char*)malloc(sizeof(36));

	if(conectado == 0) log_info(logPlanificador,"Conexion en socket %d cerrada\n",socket);

	else log_info(logPlanificador,"Nueva coneccion en socket %d\n",socket);

	free(logueo);
}

void logueoAlgoritmo(char inicial,char* mProc){//TODO tiene que loguear todas las colas y listas??
	char* logueo = (char*)malloc(65+strlen(mProc));
	char* algoritmo = string_new();
	if(inicial == 'F') strcpy(algoritmo,"FIFO");
	else strcpy(algoritmo,"RR");
	log_info(logPlanificador,"El Proceso %s se encuentra ejecutando algoritmo %s\n",nombrePrograma(mProc),algoritmo);
	free(algoritmo);
	free(logueo);
}

void logueoRafaga(){
//TODO ??? no entendi
}
