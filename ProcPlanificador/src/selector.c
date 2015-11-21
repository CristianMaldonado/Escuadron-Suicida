#include "estructuras.h"
#include "funcionesPlanificador.h"
#include "libSocket.h"
#include <commons/collections/list.h>
#include <pthread.h>
#include <commons/string.h>
#include "logueo.h"

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
							pthread_mutex_lock(&mutexSwitchProc);
							pthread_mutex_lock(&mutexListaEjecutando);
							aux->pcb = list_remove(listaEjecutando,buscoPCB(respuestaDeCPU.pid,listaEjecutando));
							pthread_mutex_unlock(&mutexListaEjecutando);
							aux->pcb->siguiente = respuestaDeCPU.counterProgram;
							aux->tiempo = atoi(respuestaDeCPU.mensaje);
							aux->pcb->estado = IO;
							pthread_mutex_lock(&mutexIO);
							queue_push(colaIO,aux);
							pthread_mutex_unlock(&mutexIO);
							pthread_mutex_unlock(&mutexSwitchProc);

							sem_post(&hayIO);
							sem_post(&hayCPU);
							printf("pid-> %d entro a io\n", respuestaDeCPU.pid);
						}
						break;

						case 'i':{
							tpcb* pcb;
							pthread_mutex_lock(&mutexSwitchProc);
							pthread_mutex_lock(&mutexInicializando);
							pcb = list_remove(listaInicializando,buscoPCB(respuestaDeCPU.pid,listaInicializando));
							pthread_mutex_unlock(&mutexInicializando);
							pcb->estado = EJECUTANDO;
							pthread_mutex_lock(&mutexListaEjecutando);
							list_add(listaEjecutando,pcb);
							pthread_mutex_unlock(&mutexListaEjecutando);
							pthread_mutex_unlock(&mutexSwitchProc);
							logueoProcesos(respuestaDeCPU.pid,respuestaDeCPU.mensaje,'i');
							logueoAlgoritmo(respuestaDeCPU.quantum,respuestaDeCPU.mensaje);
							printf("pid-> %d inicio correctamente\n", respuestaDeCPU.pid);

						}
						break;

						/*aca se usa tipo proceso*/
						case 'a':{
							int* puntero = malloc(sizeof(int));
							*puntero = socketCpu[i];
							pthread_mutex_lock(&mutexListaCpus);
							/*agreo la cpu a lista disponible*/
							list_add(listaCpuLibres,puntero);
							pthread_mutex_unlock(&mutexListaCpus);
							sem_post(&hayCPU);

							/*es un fallo de inicializacion*/
							if (respuestaDeCPU.tipoProceso == 'i'){
								pthread_mutex_lock(&mutexSwitchProc);
								pthread_mutex_lock(&mutexInicializando);
								free(list_remove(listaInicializando,buscoPCB(respuestaDeCPU.pid,listaInicializando)));
								pthread_mutex_unlock(&mutexInicializando);
								pthread_mutex_unlock(&mutexSwitchProc);
								printf("pid-> %d fallo la inicializacion\n", respuestaDeCPU.pid);
							}

							/*es un fallo de lectura o escritura*/
							else {
								pthread_mutex_lock(&mutexSwitchProc);
								pthread_mutex_lock(&mutexListaEjecutando);
								free(list_remove(listaEjecutando,buscoPCB(respuestaDeCPU.pid,listaEjecutando)));
								pthread_mutex_unlock(&mutexListaEjecutando);
								pthread_mutex_unlock(&mutexSwitchProc);
								printf("pid-> %d proceso abortado: fallo la %s\n", respuestaDeCPU.pid, (respuestaDeCPU.tipoProceso == 'l' ? "lectura":"escritura"));
							}
						}
						break;

						case 'q':{
							tpcb* pcb;
							int* puntero = malloc(sizeof(int));
							*puntero = socketCpu[i];
							pthread_mutex_lock(&mutexListaCpus);
							/*agreo la cpu a lista disponible*/
							list_add(listaCpuLibres, puntero);
							pthread_mutex_unlock(&mutexListaCpus);

							pthread_mutex_lock(&mutexSwitchProc);
							pthread_mutex_lock(&mutexListaEjecutando);
							pcb = list_remove(listaEjecutando,buscoPCB(respuestaDeCPU.pid,listaEjecutando));
							pthread_mutex_unlock(&mutexListaEjecutando);
							pcb->siguiente = respuestaDeCPU.counterProgram;
							pcb->estado = LISTO;

							pthread_mutex_lock(&mutexFinalizarPid);
								if(hayQueFinalizarlo(pcb->pid,listaAfinalizar))
									pcb->siguiente = pcb->maximo;
							pthread_mutex_unlock(&mutexFinalizarPid);

							pthread_mutex_lock(&mutexProcesoListo);
							queue_push(colaListos,pcb);
							pthread_mutex_unlock(&mutexProcesoListo);
							pthread_mutex_unlock(&mutexSwitchProc);

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
							pthread_mutex_lock(&mutexSwitchProc);
							pthread_mutex_lock(&mutexListaEjecutando);
							free(list_remove(listaEjecutando,buscoPCB(respuestaDeCPU.pid,listaEjecutando)));
							pthread_mutex_unlock(&mutexListaEjecutando);
							pthread_mutex_unlock(&mutexSwitchProc);
							sem_post(&hayCPU);
							logueoProcesos(respuestaDeCPU.pid,respuestaDeCPU.mensaje,'f');
							printf("pid-> %d finalizo\n", respuestaDeCPU.pid);
						}
						break;
						case 'u':{
							tPorcentajeCpu* porcentaje = malloc(sizeof(tPorcentajeCpu));
							porcentaje->tid = respuestaDeCPU.pid;
							porcentaje->porcentaje = respuestaDeCPU.counterProgram;
							list_add(listaPorcentajeCpus,porcentaje);

							if(list_size(listaPorcentajeCpus) == list_size(listaCpus)){
								int i;
								for(i = 0; i < list_size(listaPorcentajeCpus); i++){
									tPorcentajeCpu* aMostrar = list_get(listaPorcentajeCpus,i);
									printf("CPU %d : %d %c\n",aMostrar->tid,aMostrar->porcentaje,37);
								}

								list_clean_and_destroy_elements(listaPorcentajeCpus,free);
							}
						}break;
					}
				}
				else
				{

					/*lo seteo en -1 para luego eliminarlo del vector*/
					logueoConexionCPUS(socketCpu[i]);
					pthread_mutex_lock(&mutexComandoCpu);
					int j;
					for(j = 0; j < list_size(listaCpus); j++){
						int* cpu = list_get(listaCpus,j);
						if(*cpu == socketCpu[i]) {
							free(list_remove(listaCpus,j));
							break;
						}
					}
					pthread_mutex_unlock(&mutexComandoCpu);
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

			logueoConexionCPUS(nuevaCpu);
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

				int* p = malloc(sizeof(int));
				*p = nuevaCpu;
				pthread_mutex_lock(&mutexComandoCpu);
				list_add(listaCpus,p);
				pthread_mutex_unlock(&mutexComandoCpu);

				printf("Nueva CPU conectada: %d\n", nuevaCpu);
			}
		}
	}
	pthread_exit(0);
	return 0;
}


