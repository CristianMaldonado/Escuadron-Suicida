#include <pthread.h>
#include "../../lib/libSocket.h"
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>

#define PACKAGESIZE 30

typedef struct {
	char* ipPlanificador;
	char* puertoPlanificador;
	char* ipMemoria;
	char* puertoMemoria;
	int cantidadHilos;
	int retardo;
} tipoConfiguracionCPU;

// funcion que obtiene los campos del archivo de configuracion del cpu
tipoConfiguracionCPU* leerConfiguracion() {
	tipoConfiguracionCPU* datosCPU = malloc(sizeof(tipoConfiguracionCPU));
	t_config* config;
	config = config_create("../src/cpu.cfg");
	datosCPU->cantidadHilos = atoi(
			config_get_string_value(config, "CANTIDAD_HILOS"));
	datosCPU->ipPlanificador = config_get_string_value(config,
			"IP_PLANIFICADOR");
	datosCPU->ipMemoria = config_get_string_value(config, "IP_MEMORIA");
	datosCPU->puertoMemoria = config_get_string_value(config, "PUERTO_MEMORIA");
	datosCPU->puertoPlanificador = config_get_string_value(config,
			"PUERTO_PLANIFICADOR");
	datosCPU->retardo = atoi(config_get_string_value(config, "RETARDO"));
	return datosCPU;
}

int main() {
	system("clear");

	// creacion de la instancia de log
	t_log *logCpu = log_create("../src/log.txt", "cpu.c", false, LOG_LEVEL_INFO);

	tipoConfiguracionCPU *config = leerConfiguracion();

	/*Inicia el Socket para conectarse con el Planificador*/
	int socketPlanificador;
	printf("Conectando al Planificador (%s:%s)... ", config->ipPlanificador, config->puertoPlanificador);
	client_init(&socketPlanificador, config->ipPlanificador, config->puertoPlanificador);
	printf("OK\n");

	//loguea conexion con Planificador
	log_info(logCpu, "Conectado al Planificador");

	/*Inicia el Socket para conectarse con la Memoria*/

	int socketMemoria;
	printf("Conectando a la Memoria (%s:%s)... ", config->ipMemoria, config->puertoMemoria);
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
	return 0;
}
