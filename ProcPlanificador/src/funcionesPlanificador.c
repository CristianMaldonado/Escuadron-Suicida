#include "estructuras.h"
#include <commons/string.h>
#include <semaphore.h>
#include <stdio.h>


int maxLineas(char* path){
	FILE* archivo = fopen(path, "r+");
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

tpcb* armarPCB(char* path, int cant) {//OK
	tpcb* pcb = malloc(sizeof(tpcb));
	pcb->ruta = (char*) malloc(strlen(path)+1);
	strcpy(pcb->ruta, path);
	path[strlen(path)-1] = '\0';
	pcb->pid = cant;
	pcb->nombre = string_new();
	pcb->estado = LISTO;
	pcb->siguiente = 1;
	pcb->maximo= maxLineas(path);
	return pcb;
}

/*void finalizarPID(char* pidBuscado,t_queue* colaProc){
	t_list* lista= (colaProc)->elements;
	t_link_element* element = lista->head;
	tpcb* pcb;
	int position = 0;
	while (element != NULL){
		pcb=(element->data);
		//TODO : estan comparando un entero con un char*
		//if((pcb->pid)!=pidBuscado) {
			element=element->next;
			position++;
		}
		else{
			exit(1);
		}
	}
	pcb->siguiente=pcb->maximo;
}
*/
void convertirEstado(testado estadoEnum, char* estado){

if (estadoEnum == LISTO) {
	estado = malloc(7);
	strcpy(estado, "LISTO");
}
if (estadoEnum == IO) {
	estado = malloc(7);
	strcpy(estado, "IO");
}
if (estadoEnum == EJECUTANDO) {
	estado = malloc(12);
	strcpy(estado, "EJECUTANDO");
}
if (estadoEnum == FINALIZADO) {
	estado = malloc(12);
	strcpy(estado, "FINALIZADO");
}

 }

void mostrarEstadoProcesos(t_queue* colaProc){
	char* infoProceso = (char*)malloc(50);

	t_list* lista= (colaProc)->elements;
	t_link_element* element = lista->head;
	tpcb* pcb;
	int pos= 0;
	while (element != NULL){
		char* estado = string_new();
			pcb=(element->data);
				element=element->next;
				convertirEstado(pcb->estado, estado);
				strcpy(infoProceso, "mProc: ");
				string_append_with_format(&infoProceso, "%d ", pcb->pid);
				string_append(&infoProceso, pcb->nombre);
				string_append(&infoProceso, " -> ");
				string_append_with_format(&infoProceso, "%d", estado);
				printf("%s",infoProceso);

				free(estado);
				pos++;

		}


    free(infoProceso);
}

void mostrarEstadoProcesosLista(t_list* lista){
	char* infoProceso = (char*)malloc(50);

	t_link_element* element = lista->head;
	tpcb* pcb;
	int pos= 0;
	while (element != NULL){
		char* estado = string_new();
			pcb=(element->data);
				element=element->next;
				convertirEstado(pcb->estado, estado);
				strcpy(infoProceso, "mProc: ");
				string_append_with_format(&infoProceso, "%d ", pcb->pid);
				string_append(&infoProceso, pcb->nombre);
				string_append(&infoProceso, " -> ");
				string_append_with_format(&infoProceso, "%d", estado);
				printf("%s",infoProceso);

				free(estado);
				pos++;

		}


    free(infoProceso);
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

void procesarComando(int nro_comando, char* message, int* cantProc,t_queue* colaProc) {//OK
	tpcb* pcb;
	switch (nro_comando) {
	case 1:
		printf("Entro por ps\n");
		mostrarEstadoProcesos(colaProc);
		break;
	case 2:
		printf("Entro por cpu\n");
		break;
	case 3:
		pcb = armarPCB(&message[7], *cantProc);//TODO cambiar como el interpretar instruccion
		queue_push(colaProc, pcb);
		(*cantProc) = (*cantProc)+ 1;
		sem_post(&hayProgramas);
		break;
	case 4:
	//	finalizarPID(&message[10],colaProc);
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
	paquete->quantum = 0;
	paquete->tamanioMensaje = strlen(pcb->ruta)+1;
	paquete->mensaje =malloc(strlen(pcb->ruta)+1);
	strcpy(paquete->mensaje, pcb->ruta);
}

void* serializarPaqueteCPU(protocolo_planificador_cpu* paquete, int* tamanio){ //malloc(1)
	//SERIALIZA SOLO CORRER
	size_t messageLength = strlen(paquete->mensaje);

	void* paqueteSerializado = malloc(sizeof(protocolo_planificador_cpu) + messageLength);
	int offset = 0;
	int size_to_send;

	size_to_send = sizeof(paquete->tipoProceso);
	memcpy(paqueteSerializado + offset, &(paquete->tipoProceso),size_to_send);
	offset += size_to_send;
    printf("%c\n", paquete->tipoProceso);
	size_to_send = sizeof(paquete->tipoOperacion);
	memcpy(paqueteSerializado + offset, &(paquete->tipoOperacion),size_to_send);
	offset += size_to_send;
	printf("%c\n", paquete->tipoOperacion);
	size_to_send = sizeof(paquete->estado);
	memcpy(paqueteSerializado + offset, &(paquete->estado), size_to_send);
	offset += size_to_send;
	printf("%d\n", paquete->estado);
	size_to_send = sizeof(paquete->pid);
	memcpy(paqueteSerializado + offset, &(paquete->pid), size_to_send);
	offset += size_to_send;
	printf("%d\n", paquete->pid);
	size_to_send = sizeof(paquete->counterProgram);
	memcpy(paqueteSerializado + offset, &(paquete->counterProgram),size_to_send);
	offset += size_to_send;
	printf("%d\n", paquete->counterProgram);
	size_to_send = sizeof(paquete->quantum);
	memcpy(paqueteSerializado + offset, &(paquete->quantum), size_to_send);
	offset += size_to_send;
	printf("%d\n", paquete->quantum);
	size_to_send = sizeof(paquete->tamanioMensaje);
	memcpy(paqueteSerializado + offset, &messageLength, size_to_send);
	offset += size_to_send;
	printf("%d\n", paquete->tamanioMensaje);
	size_to_send = messageLength;
	memcpy(paqueteSerializado + offset, paquete->mensaje, size_to_send);
	offset += size_to_send;
	printf("%s\n", paquete->mensaje);


	*tamanio = offset;
	return paqueteSerializado;

}


