#include <stdio.h>
#include "../../lib/libSocket.h"
#define PACKAGESIZE 30

int main(){
/*Inicia el socket para escuchar*/
	int serverSocket;
	if(server_init(&serverSocket, "4143"))
		printf("Planificador listo\n");
/*Inicia el socket para atender al CPU*/
	int socketCPU;
	if(server_acept(serverSocket, &socketCPU))
		printf("CPU aceptado\n");

	int enviar = 1;
	char message[PACKAGESIZE];

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message,"exit\n")) enviar = 0;
		if (enviar) send(socketCPU, message, strlen(message) + 1, 0);
	}

	close(serverSocket);
	close(socketCPU);

	return 0;
}
