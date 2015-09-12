#include <pthread.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>
#include "estructuras.h"

#define PACKAGESIZE 30



int main() {

	system("clear");

	// creacion de la instancia de log
	t_log *logMemoria = log_create("../src/log.txt", "memoria.c", false, LOG_LEVEL_INFO);

	//Leemos datos del archivo de configuracion
	tconfig_memoria * config = leerConfiguracion();

	/*Definimos datos Cliente listener */
	int socketClienteSWAP;
	printf("Conectando al SWAP (%s : %s)... ", config->ipSwap, config->puertoEscucha);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoEscucha);
	printf("OK\n");

	// loguea conexion con SWAP
	log_info(logMemoria, "Conectado al SWAP");

	/* Definimos datos Server*/
	int socketServidorCPU;
	server_init(&socketServidorCPU, "4142");
	printf("Memoria lista...\n");

	//loguea el inicio de la memoria
	log_info(logMemoria, "Memoria iniciada");

	int socketClienteCPU;
	server_acept(socketServidorCPU, &socketClienteCPU);
	printf("CPU aceptado...\n");

	/*Pasaje de mensaje*/
	char package[PACKAGESIZE];
	int status = 1;

	while (status != 0) {
		status = recv(socketClienteCPU, (void*) package, PACKAGESIZE, 0);
		if (status)
			send(socketClienteSWAP, package, strlen(package) + 1, 0);
		if (status)
			printf("%s", package);
	}

	printf("Finalizo el planificador...\n");

	close(socketClienteSWAP);
	log_info(logMemoria, "cerrada la conexion con Swap");
	close(socketClienteCPU);
	close(socketServidorCPU);
	log_info(logMemoria, "Memoria finalizada");

	return 0;
}
