#include <stdio.h>
#include "../../lib/libSocket.h"
#define PACKAGESIZE 30

int main() {

	int serverSocket;
	if(server_init(&serverSocket, "8888"))
		printf("conexion establecida con CPU\n");

	int socketCli;
	if (client_init(&socketCli,"192.168.1.1", "8888"))
		printf("no hubo error");

	if(server_acept(serverSocket, &socketCli))
		printf("CPU aceptado\n");


	int enviar = 1;
	char message[PACKAGESIZE];

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message,"exit\n")) enviar = 0;
		if (enviar) send(socketCli, message, strlen(message) + 1, 0);
	}

		close(serverSocket);

		close(socketCli);



	return 0;
}
