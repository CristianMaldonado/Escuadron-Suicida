#include <pthread.h>
#include "../../lib/libSocket.h"
#include<stdio.h>
#define PACKAGESIZE 30


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
			status = recv(socketPlanificador, (void*) package, PACKAGESIZE, 0);
			if(status) send(socketMemoria,package, strlen(package) + 1,0);
			if (status) printf("%s",package);
	}

	printf("Finalizo el planificador...\n");

	close(socketMemoria);
	close(socketPlanificador);
	return 0;
}
