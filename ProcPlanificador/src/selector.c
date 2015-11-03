#include "selector.h"
#include "estructuras.h"

void *selector(void* arg) {
	fd_set master;   // conjunto maestro de descriptores de fichero
	fd_set read_fds; // conjunto temporal de descriptores de fichero para select()
	struct sockaddr_in addr; // dirección del servidor
	socklen_t addrlen = sizeof(addr); // dirección del cliente
	int fdmax;        // número máximo de descriptores de fichero
	int listener;     // descriptor de socket a la escucha
	int newfd;        // descriptor de socket de nueva conexión aceptada
	char buf[256];    // buffer para datos del cliente
	int nbytes;
	int yes = 1;        // para setsockopt() SO_REUSEADDR, más abajo
	int i, j;

	tParametroSelector* parametros;
    parametros = (tParametroSelector*) arg;
	FD_ZERO(&master);    // borra los conjuntos maestro y temporal
	FD_ZERO(&read_fds);

	struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;

	listener=parametros->socket;

	// añadir listener al conjunto maestro
	FD_SET(listener, &master);

	// seguir la pista del descriptor de fichero mayor
	fdmax = listener; // por ahora es éste

	// bucle principal
	while (1) {
		read_fds = master; // cópialo
		if (select(fdmax + 1, &read_fds, NULL, NULL, &tv) == -1) {
			perror("Error en el select");
			exit(1);
		}

		// explorar conexiones existentes en busca de datos que leer
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // ¡¡tenemos datos!!
				if (i == listener) {
					// gestionar nuevas conexiones
					if ((newfd = accept(listener,
							(struct sockaddr *) &addr, &addrlen)) == -1) {
						perror("accept");
					} else {
						FD_SET(newfd, &master); // añadir al conjunto maestro
						list_add(parametros->listaCpus,newfd);
						if (newfd > fdmax) {    // actualizar el máximo
							fdmax = newfd;
						}
						printf("selectserver: new connection from %s on "
								"socket %d\n", inet_ntoa(addr.sin_addr),
								newfd);
					}
				} else {
					// gestionar datos de un cliente

					if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
						// error o conexión cerrada por el cliente
						if (nbytes == 0) {
							// conexión cerrada
							printf("selectserver: socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						close(i); // bye!
						FD_CLR(i, &master); // eliminar del conjunto maestro
					}
					else {
						// tenemos datos de algún cliente
						//for (j = 0; j <= fdmax; j++) {
						//	if (FD_ISSET(j, &master)) {
								//TODO gestionar llegada

						/*	status = deserializarCPU(respuestaDeCPU,i);
							if(status == 0) error_show("Desconexion de CPU");
							switch(respuestaDeCPU->tipoOperacion){

								case 'i':{//METE EN LISTA EJECUTANDO
								}break;

								case 'a':{//SI FALLA LIBERA LA CPU MOVER DE COLA EJECUTANDO A COLA DISPONIBLE
										list_add(parametros->listaCpus,i);
								}break;

								case 'f':{// LIBERAR CPU MOVER DE COLA EJECUTANDO A COLA DISPONIBLE
										list_add(parametros->listaCpus,i);
								}break;
							}*/

								list_add(parametros->listaCpus,i);
							//}
						//}
					}
				} // Esto es ¡TAN FEO!
			}
		}
	}

	//return 0;
}

