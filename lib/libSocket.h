/*
 * libSocket.h
 *
 *  Created on: 2/9/2015
 *      Author: utnso
 */

#ifndef LIB_LIBSOCKET_H_
#define LIB_LIBSOCKET_H_

#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<unistd.h>
#include<stdbool.h>


void socket_close(int socket){
	close(socket);
}

bool client_init(int * cliSocket, char *ip, char *puerto){

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	/*carga en serverInfo los datos de la conexion*/
	if (getaddrinfo(ip, puerto, &hints, &serverInfo))
		return false;

	/*creamos el socket*/
	*cliSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (*cliSocket == -1)
		return false;

	/*conectamos al servidor*/
	if (connect(*cliSocket, serverInfo->ai_addr, serverInfo->ai_addrlen) == -1){
	 	socket_close(*cliSocket);
	 	return false;
	}

	/*liberamos*/
	freeaddrinfo(serverInfo);
	return true;
}

bool server_init(int *svrSocket, char *puerto){

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	/*carga en serverInfo los datos de la conexion*/
	if(getaddrinfo(NULL, puerto, &hints, &serverInfo))
		return false;

	/*creamos el socket*/
	*svrSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (*svrSocket == -1)
		return false;

        int opcion = 1;
        setsockopt(svrSocket,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option));

	if (bind(*svrSocket,serverInfo->ai_addr, serverInfo->ai_addrlen) == -1){
		socket_close(*svrSocket);
	 	return false;
	}

	/*liberamos*/
	freeaddrinfo(serverInfo);

	if(listen(*svrSocket, SOMAXCONN) == -1){
		socket_close(*svrSocket);
	 	return false;
	}

	return true;
}

bool server_acept(int serverSocket, int *clientSocket){

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	return (*clientSocket = accept(serverSocket, (struct sockaddr *) &addr, &addrlen)) != -1;
}



#endif /* LIB_LIBSOCKET_H_ */
