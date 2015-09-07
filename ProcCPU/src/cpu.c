#include <pthread.h>
#include <stdio.h>
#include <commons/config.h>
#include "../../lib/libSocket.h"

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
tipoConfiguracionCPU* leerConfiguracion(){
	tipoConfiguracionCPU* datosCPU = malloc(sizeof(tipoConfiguracionCPU));
	t_config* config;
	config = config_create("../src/cpu.cfg");
	datosCPU->cantidadHilos = atoi(config_get_string_value(config, "CANTIDAD_HILOS"));
	datosCPU->ipPlanificador = config_get_string_value(config,"IP_PLANIFICADOR");
	datosCPU->ipMemoria = config_get_string_value(config,"IP_MEMORIA");
	datosCPU->puertoMemoria = config_get_string_value(config,"PUERTO_MEMORIA");
	datosCPU->puertoPlanificador = config_get_string_value(config,"PUERTO_PLANIFICADOR");
	datosCPU->retardo = atoi(config_get_string_value(config,"RETARDO"));
	return datosCPU;
}

int main(){

	tipoConfiguracionCPU *config = leerConfiguracion();
	printf("%s\n", config->ipMemoria);

	/*Inicia el Socket para conectarse con el Planificador*/
	int socketPlanificador;
	if (client_init(&socketPlanificador,"127.0.0.1", "4143"))
		printf("Conectado al Planificador...\n");

	/*Inicia el Socket para conectarse con la Memoria*/
	int socketMemoria;
	if (client_init(&socketMemoria,"127.0.0.1", "4142"))
		printf("Conectado a la Memoria...\n");

	/*Pasaje de mensaje*/
	char package[PACKAGESIZE];
	int status=1;

	while (status != 0){

		if (!socket_recv_all(socketPlanificador, (void*)package, PACKAGESIZE))
			status = 0;

		if(status)
			socket_send_all(socketMemoria,package, strlen(package) + 1);

		if (status)
			printf("%s",package);
	}

	printf("Finalizo el planificador...\n");

	socket_close(socketMemoria);
	socket_close(socketPlanificador);
	return 0;
}
