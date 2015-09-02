#include <stdio.h>
#include "../../lib/libSocket.h"

int main() {

	int clienteSocket;
	/* abierto socket, para el cliente (planificador) */
	if(!client_init(&clienteSocket, "127.0.0.1", "8888"))
		printf("Conectado al CPU. Bienvenido, ya puede enviar mensajes. Escriba 'exit' para salir\n");


	char cadena[10] = "hola Cpu";

	send(clienteSocket, cadena, 10 , 0);
	close(clienteSocket);

	return 0;
}
