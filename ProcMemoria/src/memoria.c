#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lib/libSocket.h"
#define PACKAGESIZE 30

int main() {

	/* Definimos datos Server  */
	int serverSocket;
	if (server_init(&serverSocket, "7771"))
		printf("conexion establecida");

	int socketCliCpu;
	if (server_acept(serverSocket, &socketCliCpu))
		printf("aceptada CPU");

	// conexion con el swap
	int socketCliSwap;
	if (client_init(&socketCliSwap,"127.0.0.1", "8881"))
		printf("no hubo error");


	char package[PACKAGESIZE];
	int status = 1;
	int enviar = 1;

	while (status != 0){
		status = recv(socketCliCpu, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
		// esto que recibe lo tiene que enviar a la swap

		if (!strcmp(package,"exit\n")) enviar = 0;
		if (enviar) send(socketCliSwap, package, strlen(package) + 1, 0);

	}


	close(serverSocket);
	close(socketCliSwap);
	close(socketCliCpu);

	return 0;
}
