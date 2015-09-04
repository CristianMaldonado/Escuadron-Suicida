#include <stdlib.h>
#include <string.h>
#include "../../lib/libSocket.h"
#include<stdio.h>
#define PACKAGESIZE 30


void threadClient(){
	int cli;
	client_init(&cli, "192.168.1.1","3333");

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

	//pthread_t tServer;
	//pthread_t tClient;


	// conexion al planificador
	int socketCli;
	if (client_init(&socketCli,"127.0.0.1", "6661"))
		printf("no hubo error");


	printf("Cliente conectado. Esperando mensajes:\n");


	// conexion a la memoria
	int socketCliMemoria;
	if (client_init(&socketCliMemoria,"127.0.0.1", "7771"))
		printf("no hubo error");



	char package[PACKAGESIZE];
	int status = 1;
	int enviar = 1;

	while (status != 0){
		status = recv(socketCli, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
		// esto que recibe lo tiene que enviar a la memoria

		if (!strcmp(package,"exit\n")) enviar = 0;
		if (enviar) send(socketCliMemoria, package, strlen(package) + 1, 0);

	}



	close (socketCli);
	close(socketCliMemoria);

	return 0;
}
