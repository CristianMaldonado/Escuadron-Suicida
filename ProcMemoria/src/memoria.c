#include <pthread.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include <stdio.h>

#define PACKAGESIZE 30

typedef struct {
	char* puertoEscucha;
	char* ipSwap;
	char* puertoSwap;
	int maximoMarcosPorProceso;
	int cantidadMarcos;
	int tamanioMarco;
	int entradasTLB;
	char habilitadaTLB;
	int retardoMemoria;
} tconfig_memoria;

//Arma estructura del archivo en memoria
tconfig_memoria* leerConfiguracion() {
	tconfig_memoria* datosMemoria = malloc(sizeof(tconfig_memoria));
	t_config* config;
	config = config_create("../src/memoria.cfg");
	datosMemoria->puertoEscucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	datosMemoria->ipSwap = config_get_string_value(config, "IP_SWAP");
	datosMemoria->puertoSwap = config_get_string_value(config, "PUERTO_SWAP");
	datosMemoria->maximoMarcosPorProceso = atoi(
			config_get_string_value(config, "MAXIMO_MARCOS_POR_PROCESO"));
	datosMemoria->cantidadMarcos = atoi(
			config_get_string_value(config, "CANTIDAD_MARCOS"));
	datosMemoria->tamanioMarco = atoi(
			config_get_string_value(config, "TAMANIO_MARCO"));
	datosMemoria->entradasTLB = atoi(
			config_get_string_value(config, "ENTRADAS_TLB"));
	datosMemoria->habilitadaTLB = config_get_string_value(config,
			"TLB_HABILITADA")[0];
	datosMemoria->retardoMemoria = atoi(
			config_get_string_value(config, "RETARDO_MEMORIA"));
	return datosMemoria;
}

int main() {

	system("clear");

	// creacion de la instancia de log
	t_log *logMemoria = log_create("../src/log.txt", "memoria.c", false, LOG_LEVEL_INFO);

	//Leemos datos del archivo de configuracion
	tconfig_memoria * config = leerConfiguracion();

	/*Definimos datos Cliente listener */
	int socketClienteSWAP;
	printf("Conectando al SWAP (%s:%s)... ", config->ipSwap, config->puertoEscucha);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoEscucha);
	printf("OK\n");

	// loguea conexion con SWAP
	log_info(logMemoria, "Conectado al SWAP");

	/* Definimos datos Server*/
	int socketServidorCPU;
	server_init(&socketServidorCPU, "4142");
	printf("Memoria lista...\n");

	// loguea el inicio de la memoria
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
	close(socketClienteCPU);
	close(socketServidorCPU);

	return 0;
}
