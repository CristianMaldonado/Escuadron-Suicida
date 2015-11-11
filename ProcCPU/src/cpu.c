#include <pthread.h>
#include "libSocket.h"
#include <commons/config.h>
#include <commons/process.h>
#include "estructuras.h"
#include "funcionesCPU.h"
#include "logueo.h"
#include "serializacion.h"
#include "config.h"
#include <commons/error.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <commons/collections/list.h>

bool terminoPlanificador;
tipoConfiguracionCPU *config;

void * procesarInstruccion() {

	protocolo_planificador_cpu* datosParaProcesar = malloc(sizeof(protocolo_planificador_cpu));
	protocolo_cpu_memoria* mensajeAMemoria = malloc( sizeof(protocolo_cpu_memoria));
	protocolo_memoria_cpu* mensajeDeMemoria = malloc(sizeof(protocolo_memoria_cpu));
	int tid = process_get_thread_id();
	int socketPlanifAux;
	pthread_mutex_lock(&mutexSocket);

	printf("Conectando al Planificador (%s : %s)... ", config->ipPlanificador,config->puertoPlanificador);
	client_init(&socketPlanifAux, config->ipPlanificador, config->puertoPlanificador);
	printf("OK\n");

	if (socketPlanifAux == -1)
		log_info(logCpu, "Fallo al conectar con Planificador");
	else
		log_info(logCpu, "Conectado al Planificador");

	if (socketMemoria == -1)
		log_info(logCpu, "CPU %d fallo al conectar con Memoria", tid);
	else
		log_info(logCpu, "CPU %d se conecto con Memoria", tid);

	pthread_mutex_unlock(&mutexSocket);
	int status;
	while (1) {

		status = deserializarPlanificador(datosParaProcesar, socketPlanifAux);
		if(status < 0) {
			break;;
			sem_post(&ejecutaInstruccion);//TODO
		}

		pthread_mutex_lock(&mutexLogueo);
		logueoRecepcionDePlanif(datosParaProcesar,tid);
		pthread_mutex_unlock(&mutexLogueo);

		FILE* archivo = fopen(datosParaProcesar->mensaje, "r+");

		if (archivo == NULL){
			printf("Error al abrir mCod: %s\n", datosParaProcesar->mensaje);
			break;
		}

		fseek(archivo, 0, SEEK_END);
		int tamanio = ftell(archivo);
		fseek(archivo, 0, SEEK_SET);
		char* lineaLeida = malloc(tamanio);
		int quantum = 0;

		while ((!feof(archivo) && (quantum < datosParaProcesar->quantum || datosParaProcesar->quantum == 0))) {

			//calcularTamanioDeLinea(archivo,&tamanio);
			char* instruccionLeida = leerInstruccion(&(datosParaProcesar->counterProgram), lineaLeida, archivo,tamanio);
			char** linea = string_split(instruccionLeida, ";");
			printf("pid-> %d ejecutar %s\n", datosParaProcesar->pid, *linea);
			free(*linea);

			/*si no "entiende" la instruccion pasa a la siguiente*/
			if (!interpretarInstruccion(instruccionLeida, datosParaProcesar, mensajeAMemoria, socketPlanifAux))
				continue;

			if (datosParaProcesar->tipoOperacion == 'e') break;

			/*asi la comunicacion con la memoria es atomica*/
			pthread_mutex_lock(&mutex);
			enviarAMemoria(mensajeAMemoria);
			deserializarMemoria(mensajeDeMemoria, socketMemoria);
			pthread_mutex_unlock(&mutex);

			switch (mensajeDeMemoria->codOperacion){

				case 'f': {
					actualizarOperacionPaquetePlanificador(datosParaProcesar, 'f');
					enviarAPlanificador(datosParaProcesar,socketPlanifAux);

					/*me voy al eof para salir del while*/
					while(getc(archivo) != EOF);
					continue;
				}
				break;

				case 'i':{
					/*para saber desde el planifciador si el fallo es por inicializacion*/
					datosParaProcesar->tipoProceso = 'i';
					actualizarOperacionPaquetePlanificador(datosParaProcesar, mensajeDeMemoria->codAux);
					enviarAPlanificador(datosParaProcesar,socketPlanifAux);

					if (mensajeDeMemoria->codAux == 'a'){

						/*me voy al eof para salir del while*/
						while(getc(archivo) != EOF);
						continue;
					}
				}
				break;

				case 'l':
				case 'e':{
					if (mensajeDeMemoria->codAux == 'a'){
						/*para saber desde el planifciador si el fallo es por lectura o escritura*/
						datosParaProcesar->tipoProceso = mensajeDeMemoria->codOperacion;
						actualizarOperacionPaquetePlanificador(datosParaProcesar, 'a');
						enviarAPlanificador(datosParaProcesar,socketPlanifAux);
						datosParaProcesar->tipoOperacion = 'f';

						/*me voy al eof para salir del while*/
						while(getc(archivo) != EOF);
						continue;
					}
				}
				break;
			}
			sleep(config->retardo);
			quantum++;
		}

		/*es rr y salio por quantum y no por io*/
		if (datosParaProcesar->quantum != 0) {
			if (datosParaProcesar->quantum == quantum){
				if (datosParaProcesar->tipoOperacion != 'e' && datosParaProcesar->tipoOperacion != 'f'){
					actualizarOperacionPaquetePlanificador(datosParaProcesar, 'q');
					enviarAPlanificador(datosParaProcesar, socketPlanifAux);
					printf("pid-> %d salio por quantum\n", datosParaProcesar->pid);
				}
			}
		}
		free(lineaLeida);
		fclose(archivo);
	}
	free(mensajeAMemoria);
	free(mensajeDeMemoria);
	free(datosParaProcesar);
	close(socketPlanifAux);
	return 0;
}

int main() {
	system("clear");

	// creacion de la instancia de log
	logCpu = log_create("../src/log.txt", "cpu.c", false, LOG_LEVEL_INFO);
	config = leerConfiguracion();

	//Inicia el Socket para conectarse con la Memoria
	printf("Conectando a la Memoria (%s : %s)... ", config->ipMemoria,config->puertoMemoria);
	client_init(&socketMemoria, config->ipMemoria, config->puertoMemoria);
	printf("OK\n");

	pthread_t vectorHilos[config->cantidadHilos];
	pthread_attr_t atrib;
	pthread_mutex_init(&mutexSocket, NULL);
	pthread_mutex_init(&mutexLogueo, NULL);
	pthread_mutex_init(&mutex, NULL);
	sem_init(&ejecutaInstruccion, 0, 0);

	pthread_attr_init(&atrib);

	int i;
	for (i = 0; i < config->cantidadHilos; i++) {
		vectorHilos[i] = pthread_create(&vectorHilos[i], &atrib, procesarInstruccion, NULL);
	}

	sem_wait(&ejecutaInstruccion);

	printf("Finalizo el planificador...\n");
	close(socketMemoria);

	sem_destroy(&ejecutaInstruccion);
	pthread_mutex_destroy(&mutexSocket);
	pthread_mutex_destroy(&mutexLogueo);
	pthread_mutex_destroy(&mutex);
	log_info(logCpu, "Cerrada conexion saliente");
	log_destroy(logCpu);
	return 0;
}

