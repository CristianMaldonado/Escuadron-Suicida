
#include <pthread.h>
#include "../../lib/libSocket.h"



void threadClient(){
	int cli;
	client_init(&cli, "192.168.1.1","3333");

	//recv

	socket_close(cli);
}

void threadServer(){

	int svr;
	server_init(&svr, "puerto");

	int cli;
	server_acept(svr, &cli);

	//send

	socket_close(svr);
}

int main(){

	//pthread_t tServer;
	//pthread_t tClient;

	int serverSocket;
	if(server_init(&serverSocket, "8888"))
		printf("conexion establecida");

	int socketCli;
	if (client_init(&socketCli,"192.168.1.1", "8888"))
		printf("no hubo error");

	if(server_acept(serverSocket, &socketCli))
		printf("acepada");

	char package[10];

	recv(socketCli, package, 10, 0);
		printf("\n%s\n", package);



	//creamos los hilos, ojo que no se verifica si falla. falla cuando devuelve distinto de cero
	/*pthread_create(&tServer, NULL, (void*)&threadServer, NULL);
    pthread_create(&tClient, NULL, (void*)&threadClient, NULL);

    pthread_join(tServer, NULL);
    pthread_join(tClient, NULL);
    */
	return 0;
}
