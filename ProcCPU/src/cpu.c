#include <pthread.h>
#include "../../lib/libSocket.h"
#include<stdio.h>
#include <commons/config.h>
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




void threadClient(){
	int cli;
	client_init(&cli, "127.0.0.1","3333");

	//recv

	socket_close(cli);
}

void threadServer(){

	int svr;
	server_init(&svr, "puerto");

	int cli;
	server_acept(svr, &cli);

	//send

	socket_close(svr);
}

int main(){

	tipoConfiguracionCPU *config = leerConfiguracion();
	  printf("%s\n", config->ipMemoria);


/*Inicia el Socket para conectarse con el Planificador*/

	int socketPlanificador;
	client_init(&socketPlanificador,"127.0.0.1", "4143");
		printf("Conectado al Planificador...\n");

/*Inicia el Socket para conectarse con la Memoria*/

	int socketMemoria;
	client_init(&socketMemoria,"127.0.0.1", "4142");
		printf("Conectado a la Memoria...\n");
/*Pasaje de mensaje*/
	char package[PACKAGESIZE];
	int status=1;

	while (status != 0){
			status = recv(socketPlanificador, (void*) package, PACKAGESIZE, 0);
			if(status) send(socketMemoria,package, strlen(package) + 1,0);
			if (status) printf("%s",package);
	}

	printf("Finalizo el planificador...\n");

	close(socketMemoria);
	close(socketPlanificador);
	return 0;
}
