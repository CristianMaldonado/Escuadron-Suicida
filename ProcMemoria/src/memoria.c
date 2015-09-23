#include <pthread.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include "estructuras.h"
#include <string.h>

#define PACKAGESIZE 30

void des_serializar_cpu(void* buffer, tProtocolo_Cpu_Memoria *paquete_Desde_Cpu);


int main(void) {
/*
	system("clear");

	// creacion de la instancia de log
	t_log *logMemoria = log_create("../src/log.txt", "memoria.c", false, LOG_LEVEL_INFO);

	//Leemos datos del archivo de configuracion
	tconfig_memoria * config = leerConfiguracion();


	//Definimos datos Cliente listener
	int socketClienteSWAP;
	printf("Conectando al SWAP (%s : %s)... ", config->ipSwap, config->puertoEscucha);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoEscucha);
	printf("OK\n");

	// loguea conexion con SWAP
	log_info(logMemoria, "Conectado al SWAP");


	//Definimos datos Server
	int socketServidorCPU;
	server_init(&socketServidorCPU, "4142");
	printf("Memoria lista...\n");

	//loguea el inicio de la memoria
	log_info(logMemoria, "Memoria iniciada");


	int socketClienteCPU;
	server_acept(socketServidorCPU, &socketClienteCPU);
	printf("CPU aceptado...\n");

	// ya esta aceptado algun hilo de cpu
	///////////////////////////////////////////////////////////////////////////////////////////////









*/
	//mock serializado desde la cpu /////////////////////////////////////////////////////

	tProtocolo_Cpu_Memoria mock_protcolo;

	char* auxMsj = "holaChe";

	mock_protcolo.codOp = 'e';
	mock_protcolo.pid = 2;
	mock_protcolo.paginas = 1;
	mock_protcolo.mensaje = malloc(7 + 1);
	strcpy(mock_protcolo.mensaje, auxMsj);
	mock_protcolo.tamanio_mensaje = strlen(mock_protcolo.mensaje) +1;


	// convierto este struct en un stream
	size_t messageLength = strlen(auxMsj);
	void * buffer = malloc(13 + messageLength);
	memcpy(buffer, &(mock_protcolo.codOp), 1);
	memcpy(buffer + 1, &(mock_protcolo.pid), 4);
	memcpy(buffer + 5, &(mock_protcolo.paginas), 4);
	memcpy(buffer + 9, &messageLength, 4);
	memcpy(buffer + 13, auxMsj, messageLength);

	// deserializo desde la cpu
	tProtocolo_Cpu_Memoria paquete_Desde_Cpu;
	des_serializar_cpu(buffer, &paquete_Desde_Cpu);
	free(paquete_Desde_Cpu.mensaje);


	////////////////////////////////////////////////////////////////////////

	// pasaje de mensaje checkpoint 2
	// hay que recibir el paquete con el protocolo de cpu a memoria


	//size_t mensajeLongitud;
	//recv(socketClienteCPU, &mensajeLongitud, 13, 0);
	//void *mensajePosta = malloc(mensajeLongitud);
	// aca recibo el mensaje posta
	//recv(socketClienteCPU, mensajePosta, mensajeLongitud, 0);

	// deserializar y ya lo grabo en la struct, desde esta saco lo necesario para pasarselo al swap
	// hay que plantear el protocolo para pasarle lo necesario al administrador de swap



	//Pasaje de mensaje checkpoint 1

	/*
	char package[PACKAGESIZE];
	int status = 1;

	while (status != 0) {
		status = recv(socketClienteCPU, (void*) package, PACKAGESIZE, 0);
		if (status)
			send(socketClienteSWAP, package, strlen(package) + 1, 0);
		if (status)
			printf("%s", package);
	}

	*/

	printf("Finalizo el planificador...\n");



	/////////////////////////////////////////////////////////////////////////////////////////////
/*	close(socketClienteSWAP);
	log_info(logMemoria, "cerrada la conexion con Swap");
	close(socketClienteCPU);
	close(socketServidorCPU);
	log_info(logMemoria, "Memoria finalizada");
*/
	return 0;
}


void des_serializar_cpu(void* buffer, tProtocolo_Cpu_Memoria *paquete_Desde_Cpu) {

	//desde el buffer tomo parte por parte y lo copio en la estructura
	memcpy(&(paquete_Desde_Cpu->codOp), buffer ,1 );
	memcpy(&(paquete_Desde_Cpu->pid), buffer + 1, 4);
	memcpy(&(paquete_Desde_Cpu->paginas), buffer + 5, 4);
	memcpy(&(paquete_Desde_Cpu->tamanio_mensaje), buffer + 9, 4);

	paquete_Desde_Cpu->mensaje = malloc(paquete_Desde_Cpu->tamanio_mensaje + 1);
	memcpy(paquete_Desde_Cpu->mensaje, buffer + 13, paquete_Desde_Cpu->tamanio_mensaje);
	paquete_Desde_Cpu->mensaje[paquete_Desde_Cpu->tamanio_mensaje] = '\0';

}


