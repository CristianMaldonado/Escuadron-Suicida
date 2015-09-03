/*
 * swap.c
 *
 *  Created on: 3/9/2015
 *      Author: utnso
 */
#include <pthread.h>
#include "../../lib/libSocket.h"

int main(){

	int serverSocket;
	if(server_init(&serverSocket, "6666"))
		printf("conexion establecida con Swap\n");

	int socketCli;
	if (client_init(&socketCli,"192.168.1.1", "6666"))
		printf("no hubo error");

	if(server_acept(serverSocket, &socketCli))
		printf("memoria aceptada");

	char package[10];

	recv(socketCli, package, 10, 0);
		printf("\n%s\n", package);


		close(serverSocket);
		close(socketCli);

		return 0;

}
