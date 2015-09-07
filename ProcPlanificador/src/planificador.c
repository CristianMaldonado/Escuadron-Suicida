#include <stdio.h>
#include <commons/config.h>
#include "../../lib/libSocket.h"

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

  tconfig_planif *config = leerConfiguracion();
  printf("%c\n", config->algoritmo);
  printf("%s\n", config->puertoEscucha);

  /*Inicia el socket para escuchar*/
  int serverSocket;
  if(server_init(&serverSocket, "4143"))
		printf("Planificador listo...\n");

  	/*Inicia el socket para atender al CPU*/
  	int socketCPU = server_acept(serverSocket);
  	if(socketCPU)
  		printf("CPU aceptado...\n");

  	/*Pasaje de mensaje*/
  	int enviar = 1;
  	char message[PACKAGESIZE];

  	while(enviar){

  		fgets(message, PACKAGESIZE, stdin);
  		if (!strcmp(message,"exit\n")) enviar = 0;
  		if (enviar) socket_send_all(socketCPU, message, strlen(message) + 1);
  	}

  	socket_close(serverSocket);
  	socket_close(socketCPU);
  	return 0;
}

