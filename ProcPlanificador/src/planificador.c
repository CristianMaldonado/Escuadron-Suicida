#include <stdio.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include <sys/types.h>
#define PACKAGESIZE 30

typedef struct {
	char* puertoEscucha;
	char algoritmo;
	int quantum;
}tconfig_planif;

tconfig_planif *leerConfiguracion(){
	tconfig_planif *datosPlanif = malloc(sizeof(tconfig_planif));
	t_config *config;
	config = config_create("../src/planificador.cfg");
	datosPlanif->puertoEscucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	datosPlanif->algoritmo = config_get_string_value(config,"ALGORITMO_PLANIFICACION")[0];
	datosPlanif->quantum = atoi(config_get_string_value(config,"QUANTUM"));
	return datosPlanif;
}

int main(){

	system("clear");

	// creacion de la instancia de log
	t_log *logPlanificador = log_create("../src/log.txt", "planificador.c", false, LOG_LEVEL_INFO);
	logPlanificador->pid = 1;

	//Inicia el socket para escuchar
	int serverSocket;
	server_init(&serverSocket, "4143");
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

	return 0;
}

