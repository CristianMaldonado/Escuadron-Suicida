#include <stdio.h>
#include <commons/config.h>
#include "../../lib/libSocket.h"

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
}tconfig_memoria;


//Arma estructura del archivo en memoria
tconfig_memoria* leerConfiguracion(){
	tconfig_memoria* datosMemoria = malloc(sizeof(tconfig_memoria));
	t_config* config;
	config = config_create("../src/memoria.cfg");
	datosMemoria->puertoEscucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	datosMemoria->ipSwap = config_get_string_value(config,"IP_SWAP");
	datosMemoria->puertoSwap = config_get_string_value(config,"PUERTO_SWAP");
	datosMemoria->maximoMarcosPorProceso = atoi(config_get_string_value(config,"MAXIMO_MARCOS_POR_PROCESO"));
	datosMemoria->cantidadMarcos = atoi(config_get_string_value(config,"CANTIDAD_MARCOS"));
	datosMemoria->tamanioMarco = atoi(config_get_string_value(config,"TAMANIO_MARCO"));
	datosMemoria->entradasTLB = atoi(config_get_string_value(config,"ENTRADAS_TLB"));
	datosMemoria->habilitadaTLB = config_get_string_value(config,"TLB_HABILITADA")[0];
	datosMemoria->retardoMemoria = atoi(config_get_string_value(config,"RETARDO_MEMORIA"));
	return datosMemoria;
}

int main(){

	//Leemos datos del archivo de configuracion
	tconfig_memoria * config = leerConfiguracion();
	printf("%s\n", config->ipSwap);


	/*Definimos datos Cliente listener */

	int socketClienteSWAP;
	if (client_init(&socketClienteSWAP, "127.0.0.1", "4141"))
		printf("Conectado al SWAP...\n");

	/*
	char package[10]="Hola Swap";
	send(socketClienteSWAP, package, 10, 0);
	 */

	/* Definimos datos Server*/
	int socketServidorCPU;
	if (server_init(&socketServidorCPU, "4142"))
		printf("Memoria lista...\n");

	int socketClienteCPU = server_acept(socketServidorCPU);
	if (socketClienteCPU)
		printf("CPU aceptado...\n");

	/*Pasaje de mensaje*/
	char package[PACKAGESIZE];
	int status=1;

	while (status != 0){

		if (!socket_recv_all(socketClienteCPU, (void*) package, PACKAGESIZE))
			status = 0;

		if(status)
			socket_send_all(socketClienteSWAP,package, strlen(package) + 1,0);

		if (status)
			printf("%s", package);
	}

	printf("Finalizo el planificador...\n");

	socket_close(socketClienteSWAP);
	socket_close(socketClienteCPU);
	socket_close(socketServidorCPU);

	return 0;
}
