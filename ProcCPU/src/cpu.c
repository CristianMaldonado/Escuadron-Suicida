#include <pthread.h>
#include "../../lib/libSocket.h"
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include "estructuras.h"

#define PACKAGESIZE 30

void serializar(void* paquete) {
	//TODO SERIALIZARRR !!!!!!!!!!!!!!!!!!!!!!!!!
}

int clasificarComando(char* message) {
	char * comando = malloc(strlen(message) + 1);
	strcpy(comando, message);

	if (!strcmp(comando, "iniciar\0")) {
		free(comando);
		return 1;
	}

	else {

		if (!strcmp(comando, "leer\0")) {
			free(comando);
			return 2;
		}

		else {
			if (!strcmp(comando, "escribir\0")) {
				free(comando);
				return 3;
			} else {
				if (!strcmp(comando, "entrada-salida\0")) {
					free(comando);
					return 4;
				}
				if (!strcmp(comando, "finalizar\0")) {
					free(comando);
					return 5;
				} else {
					free(comando);
					return 0;

				}
			}
		}
	}

}

void armarPaquete(protocolo_cpu_memoria* aux, char codOperacion, char codAux,
		int pid, int nroPagina, char* mensaje) {

	aux->codOperacion = codOperacion;
	aux->codAux = codAux;
	aux->pid = pid;
	aux->nroPagina = nroPagina;
	strcpy(aux->mensaje, mensaje);

	//TODO Hacerlo mas genérico con un booleano y cargue la estructura (sin mandar todos los parametros)
}

void procesarComando(int nro_comando, char* instruccion, int pid,
		int socketPlanificador, int socketMemoria) { //RECIBE PID O PCB ?

	protocolo_cpu_memoria paquete;
	string_n_split(instruccion, 3, " ");

	switch (nro_comando) {
	case 1:
		armarPaquete(&paquete, 'i', '\0', pid, instruccion[1], '\0');
		serializar(&paquete);
		//send(socketMemoria, paquete, sizeof(paquete), 0); paquete es tipo estructura hay que serializar
		break;
	case 2:
		armarPaquete(&paquete, 'l', '\0', pid, instruccion[1], '\0');
		serializar(&paquete);
		//send(socketMemoria, paquete, sizeof(paquete), 0); idem
		break;

	case 3:
		armarPaquete(&paquete, 'e', '\0', pid, instruccion[1], '\0');
		serializar(&paquete);
		//send(socketMemoria, paquete, sizeof(paquete), 0);
		break;

	case 4: //TODO definir protocolo con planificador
		armarPaquete(&paquete, 'i', '\0', pid, instruccion[1], '\0');
		serializar(&paquete);
		//send(socketPlanificador, paquete, sizeof(paquete), 0);
		break;

	case 5:
		armarPaquete(&paquete, 'f', '\0', pid, 0, '\0');
		serializar(&paquete);
		//send(socketMemoria, paquete, sizeof(paquete), 0);
		break;

	default:
		printf("Comando Incorrecto");
		break;

	}
}

int main() {
	system("clear");

	pthread_t hilo;
	pthread_attr_t atrib;
//	pthread_create(&hilo, &atrib, procesarComando, parametro);

// creacion de la instancia de log
	t_log *logCpu = log_create("../src/log.txt", "cpu.c", false,
			LOG_LEVEL_INFO);

	tipoConfiguracionCPU *config = leerConfiguracion();

	/*Inicia el Socket para conectarse con el Planificador*/
	int socketPlanificador;
	printf("Conectando al Planificador (%s : %s)... ", config->ipPlanificador,
			config->puertoPlanificador);
	client_init(&socketPlanificador, config->ipPlanificador,
			config->puertoPlanificador);
	printf("OK\n");

	//loguea conexion con Planificador
	log_info(logCpu, "Conectado al Planificador");

	/*Inicia el Socket para conectarse con la Memoria*/
	int socketMemoria;
	printf("Conectando a la Memoria (%s : %s)... ", config->ipMemoria,
			config->puertoMemoria);
	client_init(&socketMemoria, config->ipMemoria, config->puertoMemoria);
	printf("OK\n");

	// loguea conexion con Memoria
	log_info(logCpu, "Conectado a la Memoria");

	/*Pasaje de mensaje*/
	char package[PACKAGESIZE];
	int status = 1;

	while (status != 0) {
		status = recv(socketPlanificador, (void*) package, PACKAGESIZE, 0);
		if (status)
			send(socketMemoria, package, strlen(package) + 1, 0);
		if (status)
			printf("%s", package);
	}

	printf("Finalizo el planificador...\n");

	close(socketMemoria);
	close(socketPlanificador);
	log_info(logCpu, "Cerrada conexion saliente");
	return 0;
}
