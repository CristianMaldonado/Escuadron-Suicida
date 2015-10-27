#include "estructuras.h"
#include <commons/string.h>
#include <semaphore.h>


int maxLineas(FILE* archivo){
	fseek(archivo,0,SEEK_SET);
	int cont=1;
	while (!feof(archivo)) {
		cont++;
	}
	return cont;
}


tpcb* armarPCB(char* path, int cant) {//OK
	tpcb* pcb = malloc(sizeof(tpcb));
	pcb->ruta = (char*) malloc(strlen(path) + 1);
	strcpy(pcb->ruta, path);
	pcb->pid = cant;
	pcb->nombre = string_new();
	pcb->estado = LISTO;
	pcb->siguiente = 1;
	pcb->maximo=maxLineas((FILE*) path);
	return pcb;
}

void finalizarPID(char* pidBuscado,t_queue* colaProc){
	t_list* lista= (colaProc)->elements;
	t_link_element* element = lista->head;
	tpcb* pcb;
	int position = 0;
	while (element != NULL){
		pcb=(element->data);
		if((pcb->pid)==pidBuscado){
			element=element->next;
			position++;
		}
		else{
			exit(1);
		}
	}
	pcb->siguiente=pcb->maximo;
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

void procesarComando(int nro_comando, char* message, int cantProc,t_queue* colaProc) {//OK
	tpcb* pcb;
	switch (nro_comando) {
	case 1:
		printf("Entro por ps\n");
		break;
	case 2:
		printf("Entro por cpu\n");
		break;
	case 3:
		pcb = armarPCB(&message[7], cantProc);//TODO cambiar como el interpretar instruccion
		queue_push(colaProc, pcb);
		cantProc++;
		sem_post(&hayProgramas);
		break;
	case 4:
		finalizarPID(&message[10],colaProc);
		break;
	default:
		printf("Comando ingresado incorrecto\n");
		break;
	}
}


int deserializarCPU(protocolo_planificador_cpu *package,int socketCPU) {
	int status;
	char* buffer = malloc(sizeof(package->tipoProceso)+ sizeof(package->tipoOperacion)+ sizeof(testado)+ sizeof(package->pid)+
			sizeof(package->counterProgram)+ sizeof(package->quantum)+ sizeof(package->tamanioMensaje));
	int offset = 0;

	status = recv(socketCPU, buffer,sizeof(package->tipoOperacion) + sizeof(package->tipoProceso), 0);
	memcpy(&(package->tipoProceso), buffer, sizeof(package->tipoProceso));
	offset += sizeof(package->tipoProceso);
	memcpy(&(package->tipoOperacion), buffer + offset, sizeof(package->tipoOperacion));
	offset += sizeof(package->tipoOperacion);

	if (!status) return 0;

	status = recv(socketCPU, buffer,sizeof(package->estado) + sizeof(package->pid)
			+ sizeof(package->counterProgram + sizeof(package->quantum) + sizeof(package->tamanioMensaje)),0);
	memcpy(&(package->estado), buffer + offset, sizeof(package->estado));
	offset += sizeof(package->estado);
	memcpy(&(package->pid), buffer + offset, sizeof(package->pid));
	offset += sizeof(package->pid);
	memcpy(&(package->counterProgram), buffer + offset, sizeof(package->counterProgram));
	offset += sizeof(package->counterProgram);
	memcpy(&(package->quantum), buffer + offset, sizeof(package->quantum));
	offset += sizeof(package->quantum);
	memcpy(&(package->tamanioMensaje), buffer + offset, sizeof(package->tamanioMensaje));
	offset += sizeof(package->tamanioMensaje);

	if (!status) return 0;

	package->mensaje = malloc((package->tamanioMensaje) +1);
	status = recv(socketCPU, buffer, package->tamanioMensaje,0);
	memcpy(&(package->mensaje), buffer + offset, package->tamanioMensaje);
	package->mensaje[package->tamanioMensaje+1]= '\0';
	if(!status) return 0;

	free(buffer);

	return status;
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


