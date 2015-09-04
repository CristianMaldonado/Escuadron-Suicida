#include <pthread.h>
#include "../../lib/libSocket.h"
#define PACKAGESIZE 30

int main(){

/*Crea el socket para escuchar*/
	int serverSocket;
	if(server_init(&serverSocket, "4141"))
		printf("SWAP listo\n");

/*Crea el socket para recibir a la memoria*/
	int socketMemoria;
	if(server_acept(serverSocket, &socketMemoria))
		printf("memoria aceptada");

	char package[10];

	recv(socketMemoria, package, 10, 0);
		printf("%s\n", package);


	char package2[PACKAGESIZE];
	int status=1;

	while (status){
		status = recv(socketMemoria, (void*) package2, PACKAGESIZE, 0);
		if (status) printf("%s", package2);
	}

	printf("Finalizo el planificador\n");

	close(serverSocket);
	close(socketMemoria);

	return 0;
}
