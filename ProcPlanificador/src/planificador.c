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

#define PACKAGESIZE 30

//t_list* listaEjecutando=list_create();
bool llegoExit;


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

void *enviar(void *arg){
	tpcb* pcb; // che por mas que lo mire supongo que te falto escribir algo este pcb esta al pedo nunca se carga y asigna nada a message
	tParametroEnviar* parametros;
	parametros = (tParametroEnviar*) arg;

	int tamanio = 0;
	int * socketCPU;
	puts("estas en el hilo bien por vos");
	while(1){
		sem_wait(&hayCPU);
		sem_wait(&hayProgramas);
		if(llegoExit) pthread_exit(0);
		puts("pasaste el semaforo");
		pcb=queue_pop(parametros->procesos);
		printf("saque pcb\n");

		protocolo_planificador_cpu* package = malloc(sizeof(protocolo_planificador_cpu));
		adaptadorPCBaProtocolo(pcb,package);
		printf("estoy por serializqar\n");

		void* message=malloc(sizeof(protocolo_planificador_cpu) + strlen(pcb->ruta));
		message = serializarPaqueteCPU(package, &tamanio);
		//message[strlen((message))] = '\0';
		socketCPU = list_remove(parametros->listaCpus, 0);
		int a = send(*socketCPU,message,tamanio,0);
		if(a == -1) printf("fallo envio %d\n", *socketCPU);
		else printf("%d\n",a);
		/*char algooo[PACKAGESIZE]; test al pedo
		recv(parametros->socket,algooo,PACKAGESIZE,0);
		printf("%s",algooo);*/
		printf("Envie paquete");
		free(socketCPU);
		free(package);
		free(message);
		//list_add(&listaEjecutando,pcb);
		free(pcb);
	}
}

int main(){
	system("clear");
	int cantProc=1;

	listaEjecutando= list_create();
	listaCpuLibres= list_create();

	pthread_t enviarAlCpu,selectorCpu;
	pthread_attr_t attr;
	sem_init(&hayProgramas,0,0);
	sem_init(&hayCPU,0,0);

	//creacion de la instancia de log
	logPlanificador = log_create("../src/log.txt", "planificador.c", false, LOG_LEVEL_INFO);
	//logPlanificador->pid = 1;

	/*tconfig_planif* configPlanificador = malloc(sizeof(tconfig_planif));
	configPlanificador->puertoEscucha = "4143";
	configPlanificador->quantum = 0;
	configPlanificador->algoritmo = "FIFO";*/


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

	/*int socketCPU;
	server_acept(serverSocket, &socketCPU);
	printf("CPU aceptado...\n");*/

	tParametroSelector sel;
	sel.socket = serverSocket;
	sel.listaCpus = listaCpuLibres;

	pthread_attr_init(&attr);
	pthread_create( &selectorCpu, &attr, selector,(void*) &sel);

	//selector(serverSocket, listaCpuLibres);

	colaProcesos=queue_create();

	tParametroEnviar envio;
	envio.listaCpus=listaCpuLibres;
	envio.procesos=colaProcesos;
	llegoExit = false;
	pthread_attr_init(&attr);
	pthread_create( &enviarAlCpu, &attr, enviar,(void*) &envio);

	int enviar2 = 1;
	char message[PACKAGESIZE];

	int nro_comando=0;

	while(enviar2){
		fgets(message, PACKAGESIZE, stdin);

		nro_comando = clasificarComando(&message[0]);
        //TODO: VER SI EL PROCESAR COMANDO TIENE QUE RECIBIR TODAS LAS LISTAS Y COLAS, O HACERLAS GLOBALES PARA EL PS
		procesarComando(nro_comando,&message[0],&cantProc,colaProcesos);
		llegoExit = false;
		nro_comando=0;

		if (!strcmp(message,"exit\n")) {
		llegoExit = true;
		sem_post(&hayProgramas);
		enviar2 = 0;
	}

	}

	pthread_join(enviarAlCpu,NULL);

	close(serverSocket);
	//close(socketCPU);
	queue_destroy(colaProcesos);
	return 0;
}
