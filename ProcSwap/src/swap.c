#include <pthread.h>
#include "../../lib/libSocket.h"
#include<stdio.h>
#define PACKAGESIZE 30

int main(){

/*Crea el socket para escuchar*/
	int serverSocket;
	if(server_init(&serverSocket, "4141"))
		printf("SWAP listo...\n");

/*Crea el socket para recibir a la memoria*/
	int socketMemoria;
	if(server_acept(serverSocket, &socketMemoria))
		printf("Memoria aceptada...\n");

/*Pasaje de mensaje*/
	char package[PACKAGESIZE];
	int status=1;

	while (status){
		status = recv(socketMemoria, (void*) package, PACKAGESIZE, 0);
		if (status) printf("%s", package);
	}

	printf("Finalizo el planificador...\n");

	close(serverSocket);
	close(socketMemoria);

	return 0;
}
