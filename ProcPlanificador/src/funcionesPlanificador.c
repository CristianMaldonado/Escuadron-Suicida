#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/types.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include "estructuras.h"

tpcb armarPCB (char* path,int cant){
	tpcb pcb;
	pcb.ruta=malloc(strlen(path)+1);
	strcpy(pcb.ruta,path);
	pcb.pid=cant;
	pcb.nombre="";
	testado std=LISTO;
	pcb.estado=std;
	pcb.siguiente=1;
	return pcb;
}

int clasificarComando(char* message){
	char* comando=malloc(3);
	memcpy(comando,message,2);
			comando[2]=0;
			if (!strcmp(comando,"ps\0")){free(comando);return 1;}
			else {
				free(comando);
				comando = (char*)malloc(4);
				memcpy(comando,message,3);
				comando[3]=0;
				if (!strcmp(comando,"cpu\0")){free(comando);return 2;}
				else {
					free(comando);
					comando = (char*)malloc(7);
					memcpy(comando,message,6);
					comando[6]=0;
					if (!strcmp(comando,"correr\0")){free(comando);return 3;}
					else {
						free(comando);
						comando = (char*)malloc(10);
						memcpy(comando,message,9);
						comando[9]=0;
						if (!strcmp(comando,"finalizar\0")){free(comando);return 4;}
						else {free(comando);return 0;}
					}
				}
			}
}

void procesarComando(int nro_comando, char* message, int cantProc,t_queue* colaProc,sem_t* sem){
	tpcb pcb;
	switch (nro_comando){
		case 1:
			printf("Entro por ps\n");
			break;
		case 2:
			printf("Entro por cpu\n");
			break;
		case 3:
			pcb= armarPCB(&message[7],cantProc);
			queue_push(colaProc,&pcb);
			cantProc++;
			sem_post(sem);
			break;
		case 4:
			printf("Entro por finalizar\n");
			break;
		default:
			printf("Comando ingresado incorrecto\n");
			break;
	}

}
