#include <pthread.h>
#include "../../lib/libSocket.h"
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include "estructuras.h"

#define PACKAGESIZE 30

int main() {
	system("clear");

	// creacion de la instancia de log
	t_log *logCpu = log_create("../src/log.txt", "cpu.c", false, LOG_LEVEL_INFO);

	tipoConfiguracionCPU *config = leerConfiguracion();

	/*Inicia el Socket para conectarse con el Planificador*/
	int socketPlanificador;
	printf("Conectando al Planificador (%s : %s)... ", config->ipPlanificador, config->puertoPlanificador);
	client_init(&socketPlanificador, config->ipPlanificador, config->puertoPlanificador);
	printf("OK\n");

	//loguea conexion con Planificador
	log_info(logCpu, "Conectado al Planificador");

	/*Inicia el Socket para conectarse con la Memoria*/
	int socketMemoria;
	printf("Conectando a la Memoria (%s : %s)... ", config->ipMemoria, config->puertoMemoria);
	client_init(&socketMemoria, config->ipMemoria, config->puertoMemoria);
	printf("OK\n");

	// loguea conexion con Memoria
	log_info(logCpu, "Conectado a la Memoria");

	/*Pasaje de mensaje*/
	char package[PACKAGESIZE];
	int status = 1;

	while (status != 0) {
		status = recv(socketPlanificador, (void*) package, PACKAGESIZE, 0);
		if (status)
			send(socketMemoria, package, strlen(package) + 1, 0);
		if (status)
			printf("%s", package);
	}

	printf("Finalizo el planificador...\n");

	close(socketMemoria);
	close(socketPlanificador);
	log_info(logCpu, "Cerrada conexion saliente");
	return 0;
}
