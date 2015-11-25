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

void logueoAlgoritmo(int inicial,char* mProc){//TODO tiene que loguear todas las colas y listas??
	char* logueo = (char*)malloc(65+strlen(mProc));
	char* algoritmo = string_new();
	if(inicial == 0) string_append(&algoritmo,"FIFO");
	else string_append(&algoritmo,"RR");
	log_info(logPlanificador,"El Proceso %s se encuentra ejecutando algoritmo %s\n",nombrePrograma(mProc),algoritmo);
	//Envio mensaje para loguear colas de planificacion
	procesarComando(1,"logueo",NULL);
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
}

void loguearColas(t_list* lista){
	tpcb* pcb;
	int i;
	if (list_is_empty(lista)){

		log_info(logPlanificador,"Esta Vacia\n");
	}
	else{
		for(i = 0; i < list_size(lista); i++){
			pcb = list_get(lista,i);

			log_info(logPlanificador,"En la posicion %d esta el proceso %s \n",i,nombrePrograma(pcb->ruta));
		}
	}
}

void loguearColaIO(t_list* lista){
	tpcb* pcb;
	int i;
	if (list_is_empty(lista)){

		log_info(logPlanificador,"Esta Vacia\n");
	}
	else{
		for(i = 0; i < list_size(lista); i++){
			pcb = ((tprocIO*)list_get(colaIO->elements,i))->pcb;

			log_info(logPlanificador,"En la posicion %d esta el proceso %s \n",i,nombrePrograma(pcb->ruta));
		}
	}
}


void loguearRafaga(){

}



void logueoAnteultimo(char* mProc, int inicial){
	pthread_mutex_lock(&mutexSwitchProc);
	char* algoritmo=string_new();
	if(inicial == 0) string_append(&algoritmo,"FIFO");
	else string_append(&algoritmo,"RR");
	//char* logueo = (char*) malloc (65+strlen(mProc));
	log_info(logPlanificador,"De acuerdo al algoritmo %s El proceso seleccionado para ejecutar es %s\n",algoritmo,nombrePrograma(mProc));
	log_info(logPlanificador,"Estado de la cola de Redy\n");
	logearColas(colaListos->elements);
	log_info(logPlanificador,"Estado de la lista de Ejecutando\n");
	logearColas(listaEjecutando);
	log_info(logPlanificador,"Estado de la lista de Inicializando\n");
	logearColas(listaInicializando);
	log_info(logPlanificador,"Estado de la cola de IO\n");
	logearColaIO(colaIO->elements);
	pthread_mutex_unlock(&mutexSwitchProc);

	//free(logueo);
	free(algoritmo);
}*/



void loguearFinalizado(tpcb* pcb){
	log_info(logPlanificador,"Proceso %s con PID: %d Finalizado\n",nombrePrograma(pcb->ruta),pcb->pid);
	log_info(logPlanificador,"Paso %d Ejecutando, %d Bloqueado, %d desde que se inicio hasta que finalizo\n",pcb->tpoCPU,pcb->tpoBloqueado,(pcb->llegada-time(NULL)));
}
