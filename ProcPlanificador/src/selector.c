#include "estructuras.h"
#include "funcionesPlanificador.h"
#include "libSocket.h"
#include <commons/collections/list.h>
#include <pthread.h>
#include <commons/string.h>

void eliminarCpusDesconectadas(int ** socketCpu, int * numeroCpus){

	t_list * auxList = list_create();
	int i;

	for (i = 0; i < *numeroCpus; i++){
		if ((*socketCpu)[i] != -1){
			int * aux = malloc(sizeof(int));
			*aux = (*socketCpu)[i];
			list_add(auxList, aux);
		}
	}

	/*actualizo cantidad de cpu conectadas*/
	*numeroCpus = list_size(auxList);

	/*cambio el tamano para solo contener las cpu conectadas*/
	if (*socketCpu){
		free(*socketCpu);
		(*socketCpu) = malloc(sizeof(int)*(*numeroCpus));
	}

	for (i = 0; i < *numeroCpus; i++){
		int * aux = list_remove(auxList, 0);
		(*socketCpu)[i] = *aux;
		free(aux);
	}

	list_destroy(auxList);
}

void agregarNuevaCpu(int nuevaCpu, int ** socketCpu, int * numeroCpus){
	/*agrando vector para colocar el nuevo socket*/
	(*socketCpu) = realloc((*socketCpu), sizeof(int)*((*numeroCpus) + 1));
	/*agrego al final la nueva cpu*/
	(*socketCpu)[*numeroCpus] = nuevaCpu;
	/*actualizo cantidad*/
	*numeroCpus = (*numeroCpus) + 1;
}

void * selector(void * arg) {

	fd_set descriptores;
	int numeroCpus = 0;
	int * socketCpu = 0;
	int i;

	int servidor = ((tParametroSelector*)arg)->socket;

	while(1){

		/*elimino las cpu con -1*/
		eliminarCpusDesconectadas(&socketCpu, &numeroCpus);

		FD_ZERO(&descriptores);

		/*añado el socket del planificador*/
		FD_SET(servidor, &descriptores);
		/*añado las cpus*/
		for (i=0; i < numeroCpus; i++)
			FD_SET (socketCpu[i], &descriptores);

		/*busco el descriptor mas grande*/
		int maxfdset = 0;

		if (numeroCpus > 1){
			maxfdset = socketCpu[0];
			for (i = 0; i < numeroCpus; i++)
				if (socketCpu[i] > maxfdset)
					maxfdset = socketCpu[i];
		}

		if (maxfdset < servidor)
			maxfdset = servidor;

		/*espero hasta que ocurra un evento*/
		if (select (maxfdset + 1, &descriptores, NULL, NULL, NULL) == -1)
			break;

		/*compruebo las cpus*/
		for (i = 0; i < numeroCpus; i++){

			if (FD_ISSET (socketCpu[i], &descriptores)){

				/*leo lo que me mando la cpu*/
				protocolo_planificador_cpu respuestaDeCPU;
				if(deserializarCPU(&respuestaDeCPU, socketCpu[i])){

					switch(respuestaDeCPU.tipoOperacion){
						case 'e':{
								tprocIO* aux = malloc(sizeof(tprocIO));
								int* puntero = malloc(sizeof(int));
								*puntero = socketCpu[i];
								pthread_mutex_lock(&mutexListaCpus);
								/*agreo la cpu a lista disponible*/
								list_add(listaCpuLibres,puntero);
								pthread_mutex_unlock(&mutexListaCpus);
								pthread_mutex_lock(&mutexListaEjecutando);
								aux->pcb = list_remove(listaEjecutando,buscoPCB(respuestaDeCPU.pid,listaEjecutando));
								pthread_mutex_unlock(&mutexListaEjecutando);
								aux->pcb->siguiente = respuestaDeCPU.counterProgram;
								aux->tiempo = atoi(respuestaDeCPU.mensaje);
								pthread_mutex_lock(&mutexIO);
								queue_push(colaIO,aux);
								pthread_mutex_unlock(&mutexIO);
								sem_post(&hayIO);
								sem_post(&hayCPU);
								printf("pid-> %d entro a io\n", respuestaDeCPU.pid);
						}
						break;

						case 'i':{
							tpcb* pcb;
							pthread_mutex_lock(&mutexInicializando);
							pcb = list_remove(listaInicializando,buscoPCB(respuestaDeCPU.pid,listaInicializando));//SUPONGO QUE SI EXISTE EL PCB ESTA EN LA LISTA
							pthread_mutex_unlock(&mutexInicializando);
							pthread_mutex_lock(&mutexListaEjecutando);
							list_add(listaEjecutando,pcb);
							pthread_mutex_unlock(&mutexListaEjecutando);
							printf("pid-> %d inicio correctamente\n", respuestaDeCPU.pid);

						}
						break;

						case 'a':{
							int* puntero = malloc(sizeof(int));
							*puntero = socketCpu[i];
							pthread_mutex_lock(&mutexListaCpus);
							/*agreo la cpu a lista disponible*/
							list_add(listaCpuLibres,puntero);
							pthread_mutex_unlock(&mutexListaCpus);
							pthread_mutex_lock(&mutexInicializando);
							free(list_remove(listaInicializando,buscoPCB(respuestaDeCPU.pid,listaInicializando)));//lo tiro a la mierda
							pthread_mutex_unlock(&mutexInicializando);
							sem_post(&hayCPU);
							printf("pid-> %d fallo\n", respuestaDeCPU.pid);
						}
						break;

						case 'q':{
							tpcb* pcb;
							pthread_mutex_lock(&mutexListaEjecutando);
							pcb = list_remove(listaEjecutando,buscoPCB(respuestaDeCPU.pid,listaEjecutando));
							pthread_mutex_unlock(&mutexListaEjecutando);
							pcb->siguiente = respuestaDeCPU.counterProgram;
							pthread_mutex_lock(&mutexProcesoListo);
							queue_push(colaListos,pcb);
							pthread_mutex_unlock(&mutexProcesoListo);
							int* puntero = malloc(sizeof(int));
							*puntero = socketCpu[i];
							pthread_mutex_lock(&mutexListaCpus);
							/*agreo la cpu a lista disponible*/
							list_add(listaCpuLibres, puntero);
							pthread_mutex_unlock(&mutexListaCpus);
							sem_post(&hayProgramas);
							sem_post(&hayCPU);
							printf("pid-> %d volvio por quantum\n", respuestaDeCPU.pid);
						}
						break;

						case 'f':{
							int* puntero = malloc(sizeof(int));
							*puntero = socketCpu[i];
							pthread_mutex_lock(&mutexListaCpus);
							/*agreo la cpu a lista disponible*/
							list_add(listaCpuLibres, puntero);
							pthread_mutex_unlock(&mutexListaCpus);
							pthread_mutex_lock(&mutexListaEjecutando);
							free(list_remove(listaEjecutando,buscoPCB(respuestaDeCPU.pid,listaEjecutando)));//termino lo tiro
							pthread_mutex_unlock(&mutexListaEjecutando);
							sem_post(&hayCPU);
							printf("pid-> %d finalizo\n", respuestaDeCPU.pid);
						}
						break;
					}
				}
				else
				{
					/*lo seteo en -1 para luego eliminarlo del vector*/
					socketCpu[i] = -1;
					printf("CPU desconectada\n");
				}
			}
		}

		/*compruebo si una cpu se quiere conectar*/
		if (FD_ISSET (servidor, &descriptores)){

			int nuevaCpu;
			/*acepto la cpu*/
			server_acept(servidor, &nuevaCpu);
			if (nuevaCpu != -1){

				/*se agrega el nuevo socket*/
				agregarNuevaCpu(nuevaCpu, &socketCpu, &numeroCpus);

				int * puntero = malloc(sizeof(int));
				*puntero = nuevaCpu;

				pthread_mutex_lock(&mutexListaCpus);
				/*agreo la cpu a lista disponible*/
				list_add(listaCpuLibres, puntero);
				pthread_mutex_unlock(&mutexListaCpus);

				sem_post(&hayCPU);

				printf("Nueva CPU conectada: %d\n", nuevaCpu);
			}
		}
	}
	return 0;
}


