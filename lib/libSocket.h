#ifndef LIB_LIBSOCKET_H_
#define LIB_LIBSOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

void socket_close(int socket){
	close(socket);
}

bool socket_send_all(int socket, char *buffer, unsigned short lng){

	char *ptr = buffer;

	while(lng>0){

		int enviados;
		if ((enviados = send(socket, ptr, lng, 0)) < 0)
			return false;

		/*avanzo en el buffer*/
		ptr += enviados;
		lng -= enviados;
	}
	return true;
}

bool socket_recv_all(int socket, char *buffer, unsigned short lng){

	/*por ahora es lo mismo, en un futuro por hay esto cambia*/
	return recv(socket, buffer, lng, 0) == lng;
}

bool client_init(int * cliSocket, char *ip, char *puerto){

	struct addrinfo hints;
	struct addrinfo *info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	/*carga en serverInfo los datos de la conexion*/
	if (getaddrinfo(ip, puerto, &hints, &info))
		return false;

	/*creamos el socket*/
	*cliSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (*cliSocket == -1)
		return false;

	/*conectamos al servidor*/
	if (connect(*cliSocket, info->ai_addr, info->ai_addrlen) == -1){
	 	socket_close(*cliSocket);
	 	return false;
	}

	/*liberamos*/
	freeaddrinfo(info);
	return true;
}

bool server_init(int *svrSocket, char *puerto){

	struct addrinfo hints;
	struct addrinfo *info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	/*carga en serverInfo los datos de la conexion*/
	if(getaddrinfo(NULL, puerto, &hints, &info))
		return false;

	/*creamos el socket*/
	*svrSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (*svrSocket == -1)
		return false;

        int option = 1;
        setsockopt(*svrSocket,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),&option,sizeof(option));

	if (bind(*svrSocket,info->ai_addr, info->ai_addrlen) == -1){
		socket_close(*svrSocket);
	 	return false;
	}

	/*liberamos*/
	freeaddrinfo(info);

	if(listen(*svrSocket, SOMAXCONN) == -1){
		socket_close(*svrSocket);
	 	return false;
	}

	return true;
}

int server_acept(int svrSocket){

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	return accept(svrSocket, (struct sockaddr *)&addr, &addrlen);
}

#endif /* LIB_LIBSOCKET_H_ */
