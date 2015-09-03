/*
 * memoria.c
 *
 *  Created on: 3/9/2015
 *      Author: utnso
 */

#include <pthread.h>
#include "../../lib/libSocket.h"

int main() {

	/* Definimos datos Server  */
	int serverSocket;
	if (server_init(&serverSocket, "7777"))
		printf("conexion establecida");

	int socketCli;
	if (client_init(&socketCli, "192.168.1.1", "7777"))
		printf("no hubo error");

	if (server_acept(serverSocket, &socketCli))
		printf("aceptada CPU");

	char package[10];

	recv(socketCli, package, 10, 0);
	printf("\n%s\n", package);

	/*Definimos datos Cliente listener */

	int clienteSocket;
	/* abierto socket, para el cliente (planificador) */
	if (!client_init(&clienteSocket, "127.0.0.1", "6666"))
		printf(
				"Conectado al SWAP. Bienvenido, ya puede enviar mensajes. Escriba 'exit' para salir\n");

	strcpy(package, "Hola SWAP");
	send(clienteSocket, package, 10, 0);

	close(clienteSocket);
	close(socketCli);

	return 0;
}
