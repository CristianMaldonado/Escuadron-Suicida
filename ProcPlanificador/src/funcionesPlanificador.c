
#include "estructuras.h"
#include <commons/string.h>

tpcb armarPCB (char* path,int cant){
	tpcb pcb;
	pcb.ruta=(char*)malloc(strlen(path)+1);
	strcpy(pcb.ruta,path);
	pcb.pid=cant;
	strcpy(pcb.nombre,"");
	pcb.estado=LISTO;
	pcb.siguiente=1;
	return pcb;
}

int clasificarComando(char* message){
			if (!strcmp(message,"ps\n")){return 1;}
			else {
				if (!strcmp(message,"cpu\n")){return 2;}
				else {
					if (string_starts_with(message,"correr")){return 3;}
					else {
						if (string_starts_with(message,"finalizar")){return 4;}
						else {return 0;}
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
