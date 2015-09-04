#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lib/libSocket.h"
#define PACKAGESIZE 30

int main() {

	int serverSocket;
	if(server_init(&serverSocket, "8888"))
		printf("conexion establecida con el puerto\n");
	// acepatamos al cpu, para enviarle mensajes

	int socketCli;
	if(server_acept(serverSocket, &socketCli))
		printf("CPU aceptado\n");


	int enviar = 1;
		char message[PACKAGESIZE];

		printf("Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");

		while(enviar){
			fgets(message, PACKAGESIZE, stdin);
			if (!strcmp(message,"exit\n")) enviar = 0;
			if (enviar) send(socketCli, message, strlen(message) + 1, 0);
	}


	close(serverSocket);
	close(socketCli);

	return 0;
}
