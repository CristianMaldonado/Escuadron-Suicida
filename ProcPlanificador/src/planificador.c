#include "funcionesPlanificador.h"
#include "estructuras.h"
#include "libSocket.h"
#include <commons/string.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>
#include "config.h"
#include "selector.h"
#include <commons/collections/queue.h>
#include <pthread.h>
#include <stdbool.h>

#define PACKAGESIZE 30

//t_list* listaEjecutando=list_create();
bool llegoexit;

void definirMensaje(tpcb* pcb,char* message){
	//char *message=malloc(strlen(pcb[0].ruta)+(2*sizeof(int))+sizeof(testado)+10+1);
	message[0]='p';
	message[1]='c';
	message[2]=pcb[0].estado;
	message[6]=pcb[0].pid;
	message[10]=pcb[0].siguiente;
	message[14]=1;
	//message[18]=sizeof(pcb[0].ruta);
	strcpy(&message[22],pcb[0].ruta);
}

void *enviar(){
	tpcb* pcb = malloc(sizeof(tpcb));
	protocolo_planificador_cpu* package = malloc(sizeof(protocolo_planificador_cpu));
	int tamanio = 0;
	int * socketCPU;

	while(1){
		sem_wait(&hayCPU);
		sem_wait(&hayProgramas);
		if(llegoexit) pthread_exit(0);
		pthread_mutex_lock(&mutexColaProcesos);
		pcb=queue_pop(colaProcesos);
		//list_add(listaAuxiliar,pcb);
		pthread_mutex_unlock(&mutexColaProcesos);

		adaptadorPCBaProtocolo(pcb,package);
		printf("estoy por serializqar\n");
		void* message=malloc(sizeof(protocolo_planificador_cpu) + strlen(pcb->ruta));

		message = serializarPaqueteCPU(package, &tamanio);
		//message[strlen((message))] = '\0';
		socketCPU = list_remove(listaCpuLibres, 0);
		int a = send(*socketCPU,message,tamanio,0);
		if(a == -1) printf("fallo envio %d\n", *socketCPU);

		free(socketCPU);
		free(message);
		//list_add(&listaEjecutando,pcb);
	}
	free(package);
	free(pcb);
}

int main(){
	system("clear");
	int cantProc=1;

	listaEjecutando= list_create();
	listaCpuLibres= list_create();
	colaProcesos = queue_create();

	pthread_t enviarAlCpu,selectorCpu;
	pthread_attr_t attr;
	sem_init(&hayProgramas,0,0);
	sem_init(&hayCPU,0,0);
	pthread_mutex_init(&mutexColaProcesos,NULL);

	//creacion de la instancia de log
	logPlanificador = log_create("../src/log.txt", "planificador.c", false, LOG_LEVEL_INFO);

	//leemos el archivo de configuracion
	configPlanificador = leerConfiguracion();

	//Inicia el socket para escuchar
	int serverSocket;
	server_init(&serverSocket, configPlanificador->puertoEscucha);
	printf("Planificador listo...\n");

	// loguea el inicio del planificador
	//log_info(logPlanificador, "planificador iniciado");

	//Inicia el socket para atender al CPU

	//list_add(listaCPU,serverSocket);

	tParametroSelector sel;
	sel.socket = serverSocket;
	sel.listaCpus = listaCpuLibres;

	pthread_attr_init(&attr);
	pthread_create( &selectorCpu, &attr, selector,(void*) &sel);

	//selector(serverSocket, listaCpuLibres);

	pthread_attr_init(&attr);
	pthread_create( &enviarAlCpu, &attr, enviar,NULL);

	int enviar2 = 1;
	char message[PACKAGESIZE];
	int nro_comando=0;
	llegoexit = false;

	while(enviar2){
		fgets(message, PACKAGESIZE, stdin);

		nro_comando = clasificarComando(&message[0]);
		procesarComando(nro_comando,&message[0],&cantProc);

		if (!strcmp(message,"exit\n")) {
			enviar2 = 0;
			llegoexit = true;
			sem_post(&hayCPU);
			sem_post(&hayProgramas);
		}

	}
	pthread_join(enviarAlCpu,NULL);
	close(serverSocket);
	//close(socketCPU);
	//list_destroy(listaAuxiliar);
	list_destroy(listaCpuLibres);
	queue_destroy(colaProcesos);
	return 0;
}
