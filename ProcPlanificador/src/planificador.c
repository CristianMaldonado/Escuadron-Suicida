#include <stdio.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include <sys/types.h>
#include "estructuras.h"

#define PACKAGESIZE 30



int main(){

	system("clear");

	//creacion de la instancia de log
	t_log *logPlanificador = log_create("../src/log.txt", "planificador.c", false, LOG_LEVEL_INFO);
	logPlanificador->pid = 1;

	//leemos el archivo de configuracion
	tconfig_planif *configPlanificador = leerConfiguracion();

	//Inicia el socket para escuchar
	int serverSocket;
	server_init(&serverSocket, configPlanificador->puertoEscucha);
	printf("Planificador listo...\n");

	// loguea el inicio del planificador
	log_info(logPlanificador, "planificador iniciado");

	//Inicia el socket para atender al CPU
	int socketCPU;
	server_acept(serverSocket, &socketCPU);
	printf("CPU aceptado...\n");

	//Pasaje de mensaje
	int enviar = 1;
	char message[PACKAGESIZE];

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message,"exit\n")) enviar = 0;
		if (enviar) send(socketCPU, message, strlen(message) + 1, 0);
	}

	close(serverSocket);
	close(socketCPU);
	log_info(logPlanificador, "planificador finalizado");
	return 0;
}

