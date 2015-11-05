#include "estructuras.h"
#include "funcionesPlanificador.h"
#include "libSocket.h"
#include <commons/collections/list.h>
#include <pthread.h>

void eliminarCpusDesconectadas(int * socketCpu, int * numeroCpus, int maxCant){

	int i;
	t_list * auxList = list_create();

	for (i = 0; i < maxCant; i++){
		if (socketCpu[i] != -1){
			int * aux = malloc(sizeof(int));
			*aux = socketCpu[i];
			list_add(auxList, aux);
		}
	}

	/*actualizo cantidad de cpu conectadas*/
	*numeroCpus = list_size(auxList);

	for (i = 0; i < *numeroCpus; i++){
		int * aux = list_remove(auxList, 0);
		socketCpu[i] = *aux;
		free(aux);
	}

	list_destroy(auxList);
}

void agregarNuevaCpu(int nuevaCpu, int * socketCpu, int * numeroCpus){

	/*agrego al final la nueva cpu*/
	socketCpu[*numeroCpus] = nuevaCpu;
	int aux = *numeroCpus;
	aux++;
	*numeroCpus = aux;
}

int * initSocket(int cant){

	int * aux = malloc(sizeof(int)*cant);

	/*inicializo con -1*/
	int i;
	for (i = 0; i < cant; i++)
		aux[i] = -1;

	return aux;
}

void * selector(void * arg) {

	fd_set descriptores;
	int numeroCpus = 0;
	//TODO hay que setear fdmaxcant con la cantidad de cpu que se van a conectar, falta en arg
	int fdmaxCant = 5;
	int i;

	int servidor = ((tParametroSelector*)arg)->socket;

	int * socketCpu = initSocket(fdmaxCant);

	while(1){

		/*elimino las cpu con -1*/
		eliminarCpusDesconectadas(socketCpu, &numeroCpus, fdmaxCant);

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

					printf("llego al select.c: %c:\n", respuestaDeCPU.tipoOperacion);
					switch(respuestaDeCPU.tipoOperacion){

						case 'a':{
								pthread_mutex_lock(&mutexListaCpus);
								int* puntero = malloc(sizeof(int));
								*puntero = socketCpu[i];
								list_add(listaCpuLibres,puntero);
								pthread_mutex_unlock(&mutexListaCpus);

								sem_post(&hayCPU);
						}
						break;

						case 'f':{
								pthread_mutex_lock(&mutexListaCpus);
								int* puntero = malloc(sizeof(int));
								*puntero = socketCpu[i];
								list_add(listaCpuLibres, puntero);
								printf("Cpu libres: %d\n", list_size(listaCpuLibres));
								pthread_mutex_unlock(&mutexListaCpus);

								sem_post(&hayCPU);
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

				agregarNuevaCpu(nuevaCpu, socketCpu, &numeroCpus);

				pthread_mutex_lock(&mutexListaCpus);
				int * puntero = malloc(sizeof(int));
				*puntero = nuevaCpu;
				/*agreo la cpu a lista disponible*/
				list_add(listaCpuLibres, puntero);
				pthread_mutex_unlock(&mutexListaCpus);

				sem_post(&hayCPU);

				printf("Nueva CPU conectada: %d\n", nuevaCpu);
			}
		}
	}
}


