#include <pthread.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include "estructuras.h"
#include <string.h>
#include "paquetes.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "config.h"
#include <stdbool.h>


#define PACKAGESIZE 30


int main(void) {

	system("clear");

	// creacion de la instancia de log
	//t_log *logMemoria = log_create("../src/log.txt", "memoria.c", false, LOG_LEVEL_INFO);

	//Leemos datos del archivo de configuracion
	tconfig_memoria * config = leerConfiguracion();


	//Definimos datos Cliente listener
	int socketClienteSWAP;
	printf("Conectando al SWAP (%s : %s)... ", config->ipSwap, config->puertoEscucha);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoEscucha);
	printf("OK\n");

	// loguea conexion con SWAP
	//log_info(logMemoria, "Conectado al SWAP");


	//Definimos datos Server
	//int socketServidorCPU;
	//server_init(&socketServidorCPU, "4142");
	//printf("Memoria lista...\n");

	//loguea el inicio de la memoria
	//log_info(logMemoria, "Memoria iniciada");


	//int socketClienteCPU;
	//server_acept(socketServidorCPU, &socketClienteCPU);
	//printf("CPU aceptado...\n");

	// ya esta aceptado algun hilo de cpu
	///////////////////////////////////////////////////////////////////////////////////////////////




	//mock serializado desde la cpu /////////////////////////////////////////////////////

	char *mensaje = "holaChe";
	tprotocolo_desde_cpu_y_hacia_swap mock_protcolo;
	armar_estructura_desde_cpu_y_hacia_swap(&mock_protcolo,'i',2,8, mensaje);

	void* buffer = malloc(13 + strlen(mensaje));
	buffer = serializar_a_swap(&mock_protcolo);

	//deserializo desde la cpu y lo meto en una struct

	//tprotocolo paquete_Desde_Cpu;
	//des_serializar_cpu(buffer, &paquete_Desde_Cpu);



	// pasaje de mensaje prueba envio del chorrro al swap

	int enviar = 1;
	while(enviar){
		scanf("%d", &enviar);
		if (enviar)
			send(socketClienteSWAP, buffer, strlen(mensaje) + 13, 0);
	}
	free(buffer);

////////////////////






	//free(paquete_Desde_Cpu.mensaje);

	// ahora tengo que interpretar los mensajes iniciar y leer,

	////////////////////////////////////////////////////////////////////////

	// pasaje de mensaje checkpoint 2
	// hay que recibir el paquete con el protocolo de cpu a memoria
	// y reenviarlo al swap


	//size_t paqueteLongitud;
	//recv(socketClienteCPU, &paqueteLongitud, 13, 0);
	//void *mensajePosta = malloc(paqueteLongitud);
	// aca recibo el mensaje posta
	//recv(socketClienteCPU, mensajePosta, paqueteLongitud, 0);

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
	close(socketClienteSWAP);
	//log_info(logMemoria, "cerrada la conexion con Swap");
	//close(socketClienteCPU);
	//close(socketServidorCPU);
	//log_info(logMemoria, "Memoria finalizada");

	return 0;
}
