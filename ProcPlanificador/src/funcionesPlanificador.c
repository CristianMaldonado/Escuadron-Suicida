#include "estructuras.h"
#include <commons/string.h>
#include <semaphore.h>
#include <stdio.h>

void* serializarPaqueteCPU(protocolo_planificador_cpu* paquete, int* tamanio){ //malloc(1)
	//SERIALIZA SOLO CORRER
	size_t messageLength = strlen(paquete->mensaje);

	void* paqueteSerializado = malloc(sizeof(protocolo_planificador_cpu) + messageLength);
	int offset = 0;
	int size_to_send;

	size_to_send = sizeof(paquete->tipoProceso);
	memcpy(paqueteSerializado + offset, &(paquete->tipoProceso),size_to_send);
	offset += size_to_send;
	size_to_send = sizeof(paquete->tipoOperacion);
	memcpy(paqueteSerializado + offset, &(paquete->tipoOperacion),size_to_send);
	offset += size_to_send;
	size_to_send = sizeof(paquete->estado);
	memcpy(paqueteSerializado + offset, &(paquete->estado), size_to_send);
	offset += size_to_send;
	size_to_send = sizeof(paquete->pid);
	memcpy(paqueteSerializado + offset, &(paquete->pid), size_to_send);
	offset += size_to_send;
	size_to_send = sizeof(paquete->counterProgram);
	memcpy(paqueteSerializado + offset, &(paquete->counterProgram),size_to_send);
	offset += size_to_send;
	size_to_send = sizeof(paquete->quantum);
	memcpy(paqueteSerializado + offset, &(paquete->quantum), size_to_send);
	offset += size_to_send;
	size_to_send = sizeof(paquete->tamanioMensaje);
	memcpy(paqueteSerializado + offset, &messageLength, size_to_send);
	offset += size_to_send;
	size_to_send = messageLength;
	memcpy(paqueteSerializado + offset, paquete->mensaje, size_to_send);
	offset += size_to_send;
	*tamanio = offset;

	return paqueteSerializado;
}

int maxLineas(char* path){
	FILE* archivo = fopen(path, "r+");
	/*desde armarPCB me fijo si es distinto de -1*/
	if (!archivo) return -1;

	int cont = 1;
	int i;
	while (!feof(archivo)) {
		for(i=0; fgetc(archivo) != '\n' && !feof(archivo);i++);
		cont++;
	}
	fclose(archivo);

	return cont;
}

char* nombrePrograma(char* path){
	int i= 0;
	char** vector = string_split(path,"/");

	while(vector[i] != NULL){
		i++;
	}

	return vector[i-1];
}

tpcb * armarPCB(char* path, int cant) {
	tpcb* pcb = malloc(sizeof(tpcb));
	pcb->ruta = (char*) malloc(strlen(path)+1);
	strcpy(pcb->ruta, path);
	path[strlen(path)-1] = '\0';
	pcb->pid = cant;
	pcb->nombre = string_new();
	pcb->estado = LISTO;
	pcb->siguiente = 1;
	pcb->maximo = maxLineas(path);
	if(pcb->maximo == -1){
		free(pcb);
		return 0;
	}
	return pcb;
}

void convertirEstado(testado estadoEnum, char** estado){

	if (estadoEnum == LISTO)
		string_append(estado, "Listo");

	if (estadoEnum == IO)
		string_append(estado, "Bloqueado");

	if (estadoEnum == EJECUTANDO)
		string_append(estado, "Ejecutando");

	if (estadoEnum == FINALIZADO)
		string_append(estado, "Finalizado");
}

void mostrarEstadoProcesosLista(t_list* lista, char* mensaje){
	char* infoProceso = (char*)malloc(50);
	tpcb* pcb;
	int i;
	for(i = 0;i < list_size(lista); i++){
		char* estado = string_new();
		pcb=list_get(lista,i);
		convertirEstado(pcb->estado, &estado);
		strcpy(infoProceso, "mProc: ");
		string_append_with_format(&infoProceso, "%d ", pcb->pid);
		string_append(&infoProceso, pcb->nombre);
		string_append(&infoProceso, "-> ");
		string_append(&infoProceso,estado);
		string_append(&infoProceso,"\n");
        //Me fijo si estoy logueando o usando el ps
		if(!strcmp(mensaje,"logueo"))
			log_info(logPlanificador,infoProceso);
		else
			printf("%s",infoProceso);
		free(estado);
	}
    free(infoProceso);
}

int buscoPCB(int pidBuscado,t_list* lista){
	tpcb* pcb;
	int posicion = 0;
	int i;
	for(i = 0; i < list_size(lista); i++){
		pcb = list_get(lista,i);
		if(pcb->pid == pidBuscado)  return posicion;
		posicion++;
	}
	return -1;
}

bool hayQueFinalizarlo(int pid,t_list* lista){

	int i;
	for(i=0 ; i<list_size(lista); i++){
		int* aux = list_get(lista,i);

		if(*aux == pid){
			free(list_remove(lista,i));
			return true;
		}
	}
    return false;
}

void finalizarPID(char* pidBuscado){
	t_list* lista= colaListos->elements;
	tpcb* pcb;
	int pid = atoi(pidBuscado);
	int posicion = buscoPCB(pid,lista);

	if(posicion == -1){
		int* aux = malloc(sizeof(int));
		*aux = pid;
		pthread_mutex_lock(&mutexFinalizarPid);
		list_add(listaAfinalizar,aux);
		pthread_mutex_unlock(&mutexFinalizarPid);
	}
	else {
		pcb = list_get(lista,posicion);
		pcb->siguiente = pcb->maximo;

	}

}

int clasificarComando(char* message) {//OK
	if (!strcmp(message, "ps\n")) {
		return 1;
	} else {
		if (!strcmp(message, "cpu\n")) {
			return 2;
		} else {
			if (string_starts_with(message, "correr")) {
				return 3;
			} else {
				if (string_starts_with(message, "finalizar")) {
					return 4;
				} else {
					return 0;
				}
			}
		}
	}
}

bool comparadorPid(void * elemA, void * elemB){

	return ((tpcb*)elemA)->pid < ((tpcb*)elemB)->pid;
}

void procesarComando(int nro_comando, char* message, int* cantProc) {//OK
	tpcb* pcb;
	switch (nro_comando){
		case 1:{
			pthread_mutex_lock(&mutexSwitchProc);

			/*lista auxiliar*/
			t_list * aux = list_create();
			int i;

			for(i = 0; i < list_size(listaEjecutando); i++)
				list_add(aux,list_get(listaEjecutando,i));

			for(i = 0; i < list_size(colaListos->elements); i++)
				list_add(aux,list_get(colaListos->elements,i));

			for(i = 0; i < list_size(colaIO->elements); i++)
				list_add(aux,((tprocIO*)list_get(colaIO->elements,i))->pcb);

			for(i = 0; i < list_size(listaInicializando); i++)
				list_add(aux,list_get(listaInicializando,i));

			/*ordeno por pid para luego mostrar*/
			list_sort(aux,comparadorPid);
			mostrarEstadoProcesosLista(aux,message);

			pthread_mutex_unlock(&mutexSwitchProc);
		}
			break;
		case 2:{
			/*int tamanio;
			int* socketCPU;
			protocolo_planificador_cpu* package = malloc(sizeof(protocolo_planificador_cpu));
			package->mensaje = malloc(1);
			package->estado = 0;
			package->tipoOperacion = 'u';
			package->tipoProceso = 'p';
			strcpy(package->mensaje,"");
			void* message=malloc(sizeof(protocolo_planificador_cpu) + strlen(package->mensaje));
			message = serializarPaqueteCPU(package, &tamanio);
			pthread_mutex_lock(&mutexListaCpus);
			socketCPU = list_get(listaCpuLibres, 0);
			send(*socketCPU,message,tamanio,0);
			pthread_mutex_unlock(&mutexListaCpus);
			free(package);
			free(message);*/
		}
			break;

		case 3:
			pcb = armarPCB(&message[7], *cantProc);
			if (pcb){
				pthread_mutex_lock(&mutexProcesoListo);
				queue_push(colaListos, pcb);
				pthread_mutex_unlock(&mutexProcesoListo);
				(*cantProc) = (*cantProc)+ 1;
				sem_post(&hayProgramas);
			}
			else
				printf("No se encontro %s\n", &message[7]);
			break;
		case 4:
			pthread_mutex_lock(&mutexSwitchProc);//TODO este va??
			pthread_mutex_lock(&mutexProcesoListo);
			finalizarPID(&message[10]);
			pthread_mutex_unlock(&mutexProcesoListo);
			pthread_mutex_unlock(&mutexSwitchProc);
			break;
		default:
			printf("Comando ingresado incorrecto\n");
			break;
	}
}

int deserializarCPU(protocolo_planificador_cpu * package, int socketCPU) {
	void* buffer = malloc(sizeof(protocolo_planificador_cpu)-4);
	if (recv(socketCPU, buffer, sizeof(protocolo_planificador_cpu)-4, 0) <= 0) return -1;
	memcpy(&(package->tipoProceso), buffer, 1);
	memcpy(&(package->tipoOperacion), buffer + 1, 1);
	memcpy(&(package->estado), buffer + 2 ,4);
	memcpy(&(package->pid), buffer + 6 ,4);
	memcpy(&(package->counterProgram), buffer + 10 ,4);
	memcpy(&(package->quantum), buffer + 14, 4);
	memcpy(&(package->tamanioMensaje), buffer + 18, 4);
	// ahora el mensaje posta
	package->mensaje = (char*)malloc(package->tamanioMensaje + 1);
	if(recv(socketCPU, package->mensaje, package->tamanioMensaje, 0) <= 0) return -1;
	package->mensaje[package->tamanioMensaje] = '\0';
	free(buffer);
	return 1;
}

void adaptadorPCBaProtocolo(tpcb* pcb,protocolo_planificador_cpu* paquete){//OK
	paquete->tipoProceso = 'p';
	paquete->tipoOperacion = 'c';
	paquete->pid = pcb->pid;
	paquete->estado = pcb->estado;
	paquete->counterProgram = pcb->siguiente;
	if(configPlanificador->algoritmo == 'F') paquete->quantum = 0;
	else paquete->quantum = configPlanificador->quantum;
	paquete->tamanioMensaje = strlen(pcb->ruta)+1;
	paquete->mensaje =malloc(strlen(pcb->ruta)+1);
	strcpy(paquete->mensaje, pcb->ruta);
}




