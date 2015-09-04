#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lib/libSocket.h"
#define PACKAGESIZE 30


int main(){

	int serverSocket;
	if(server_init(&serverSocket, "8881"))
		printf("conexion establecida con el puerto\n");
	// aceptamos a la memoria

	int socketCli;
	if(server_acept(serverSocket, &socketCli))
		printf("CPU aceptado\n");


	char package[PACKAGESIZE];
	int status = 1;

	while (status != 0){
		status = recv(socketCli, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
		// esto que recibe de la memoria
	}

	close(socketCli);
	close(serverSocket);

	return 0;

}
