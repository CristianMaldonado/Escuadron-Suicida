#include <pthread.h>
#include "../../lib/libSocket.h"
#define PACKAGESIZE 30

int main(){

/*Definimos datos Cliente listener */

	int socketClienteSWAP;
	if (client_init(&socketClienteSWAP, "127.0.0.1", "4141"))
		printf("Conectado al SWAP\n");

	char package[10]="Hola Swap";
	send(socketClienteSWAP, package, 10, 0);


/* Definimos datos Server*/
	int socketServidorCPU;
	if (server_init(&socketServidorCPU, "4142"))
		printf("Memoria lista\n");

	int socketClienteCPU;
	if (server_acept(socketServidorCPU, &socketClienteCPU))
		printf("aceptada CPU\n");

	recv(socketClienteCPU, package, 10, 0);
	printf("%s\n", package);


	char package2[PACKAGESIZE];
	int status=1;

	while (status != 0){
			status = recv(socketClienteCPU, (void*) package2, PACKAGESIZE, 0);
			if(status) send(socketClienteSWAP,package2,PACKAGESIZE,0);
			if (status) printf("%s", package2);
	}

	printf("Finalizo el planificador\n");

	close(socketClienteSWAP);
	close(socketClienteCPU);
	close(socketServidorCPU);

	return 0;
}
