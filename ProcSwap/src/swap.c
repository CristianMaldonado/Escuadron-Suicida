#include <stdio.h>
#include <commons/config.h>
#include "../../lib/libSocket.h"

#define PACKAGESIZE 30

typedef struct {
	char* puertoEscucha;
	char* nombreSwap;
	int cantidadPaginas;
	int tamanioPagina;
	int retardo;
}tconfig_swap;

//Arma estructura del archivo en swap
tconfig_swap* leerConfiguracion(){
	tconfig_swap* datosSwap = malloc(sizeof(tconfig_swap));
	t_config* config;
	config = config_create("../src/swap.cfg");
    datosSwap->puertoEscucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    datosSwap->nombreSwap = config_get_string_value(config,"NOMBRE_SWAP");
    datosSwap->cantidadPaginas = atoi(config_get_string_value(config,"CANTIDAD_PAGINAS"));
    datosSwap->tamanioPagina = atoi(config_get_string_value(config,"TAMANIO_PAGINA"));
    datosSwap->retardo = atoi(config_get_string_value(config,"RETARDO_COMPACTATION"));
	return datosSwap;
}

int main(){

	//Leemos datos del archivo de configuracion
	tconfig_swap * config = leerConfiguracion();
	printf("%s\n", config->nombreSwap); //Ejemplo

	/*Crea el socket para escuchar*/
	int serverSocket;
	if(server_init(&serverSocket, "4141"))
		printf("SWAP listo...\n");

	/*Crea el socket para recibir a la memoria*/
	int socketMemoria = server_acept(serverSocket);
	if(socketMemoria)
		printf("Memoria aceptada...\n");

	/*Pasaje de mensaje*/
	char package[PACKAGESIZE];
	int status=1;

	while (status){

		if (!socket_recv_all(socketMemoria, (void*) package, PACKAGESIZE))
			status = 0;

		if (status)
			printf("%s", package);
	}

	printf("Finalizo el planificador...\n");

	socket_close(serverSocket);
	socket_close(socketMemoria);
	return 0;
}
