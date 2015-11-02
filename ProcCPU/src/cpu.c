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

tipoConfiguracionCPU *config;
int status = 1;

void *procesarInstruccion() {

	protocolo_planificador_cpu* datosParaProcesar = malloc(sizeof(protocolo_planificador_cpu));
	//datosParaProcesar = (protocolo_planificador_cpu*) argumento;
	protocolo_cpu_memoria* mensajeAMemoria = malloc(sizeof(protocolo_cpu_memoria));
	protocolo_memoria_cpu* mensajeDeMemoria = malloc(sizeof(protocolo_memoria_cpu));
	int tid = process_get_thread_id();
	int socketPlanifAux;

	pthread_mutex_lock(&mutex);

	printf("Conectando al Planificador (%s : %s)... ", config->ipPlanificador,config->puertoPlanificador);
	//CONECCION CON PLANIFICADOR
	client_init(&socketPlanifAux, config->ipPlanificador,config->puertoPlanificador);
	printf("OK\n");

	if (socketPlanifAux == -1) log_info(logCpu, "Fallo al conectar con Planificador");
	else log_info(logCpu, "Conectado al Planificador");

	//LOGUEO DE CONEXION CON MEMORIA
	if (socketMemoria == -1) log_info(logCpu, "CPU %d fallo al conectar con Memoria", tid);
	else log_info(logCpu, "CPU %d se conecto con Memoria", tid);

	pthread_mutex_unlock(&mutex);

	while (1) {

		status = deserializarPlanificador(datosParaProcesar, socketPlanifAux);
		if(status == 0) {
			//pthread_exit(0);
			sem_post(&ejecutaInstruccion);//TODO
		}
		//pthread_mutex_lock(&mutexProceso);
		pthread_mutex_lock(&mutex);//TODO rafaga
		logueoRecepcionDePlanif(datosParaProcesar,tid);
		pthread_mutex_unlock(&mutex);

		FILE* archivo = fopen(datosParaProcesar->mensaje, "r+");
		if (archivo == NULL) error_show("Error al abrir mCod");

		fseek(archivo, 0, SEEK_END);//TODO
		int tamanio = ftell(archivo);
		fseek(archivo, 0, SEEK_SET);
		char* lineaLeida = malloc(tamanio);

		char* textoALoguear = string_new();

		int quantum = 0;

		while ((!feof(archivo) && (quantum <= datosParaProcesar->quantum || datosParaProcesar->quantum == 0))) {
			//calcularTamanioDeLinea(archivo,&tamanio);
			char* instruccionLeida = leerInstruccion(&(datosParaProcesar->counterProgram), lineaLeida, archivo,tamanio);

			printf("linea %s\n", instruccionLeida);//
			interpretarInstruccion(instruccionLeida, datosParaProcesar,mensajeAMemoria, socketPlanifAux);//si es IO arma y envia a planificador
			if (datosParaProcesar->tipoOperacion == 'E') break;
			puts("voy a enviar a memoria\n");

			pthread_mutex_lock(&mutex);
			enviarAMemoria(mensajeAMemoria);
			pthread_mutex_unlock(&mutex);

			printf("pid %d\n", mensajeAMemoria->pid);//
			printf("tamanio %d\n", mensajeAMemoria->tamanioMensaje);//
			printf("operacion %c\n", mensajeAMemoria->tipoOperacion);//

			deserializarMemoria(mensajeDeMemoria);
			string_append(&textoALoguear,prepararLogueoDeMemoria(mensajeDeMemoria));
			//prepararLogueoDeMemoria(mensajeDeMemoria,&textoALoguear);
			//pthread_mutex_lock(&mutex);//TODO rafaga
			//loguearEstadoMemoria(mensajeDeMemoria);
			//pthread_mutex_unlock(&mutex);

			printf("pid %d\n", mensajeDeMemoria->pid);//
			printf("tamanio %d\n", mensajeDeMemoria->tamanioMensaje);//
			printf("operacion %c\n", mensajeDeMemoria->codOperacion);//
			printf("cod aux %c\n", mensajeDeMemoria->codAux);//

			if (mensajeDeMemoria->codAux == 'a' && mensajeDeMemoria->codOperacion == 'i'){
				actualizarOperacionPaquetePlanificador(datosParaProcesar, 'c', 'a');
				//enviarAPlanificador();//enviar si fallo preguntar a ariel
				break;// si falla al iniciar
			}

			switch (mensajeDeMemoria->codOperacion){// arma mensaje a planificador

			 case 'i': {
				 actualizarOperacionPaquetePlanificador(datosParaProcesar, 'c', 'i');
			 }break;

			/* case 'l': {
				 actualizarOperacionPaquetePlanificador(datosParaProcesar,'l',
						 (mensajeDeMemoria->codAux == 'a')?'f' : 'c');
			 }break;

			 case 'e':{
				 actualizarOperacionPaquetePlanificador(datosParaProcesar,'e',
						 (mensajeDeMemoria->codAux == 'a')?'f' : 'c');
			 }break;*/

			 case 'f':{
				 actualizarOperacionPaquetePlanificador(datosParaProcesar,'c',
						 (mensajeDeMemoria->codAux == 'a') ? 'a' : 'f');
			 }break;


			 }
			enviarAPlanificador(datosParaProcesar,socketPlanifAux);

			sleep(config->retardo);
			quantum++;
		}

		pthread_mutex_lock(&mutex);//TODO rafaga
		log_info(logCpu,textoALoguear);
		pthread_mutex_unlock(&mutex);

		if (datosParaProcesar->quantum != 0) {
			actualizarOperacionPaquetePlanificador(datosParaProcesar, 'q',datosParaProcesar->tipoProceso);
			pthread_mutex_lock(&mutex);//TODO rafaga
			enviarAPlanificador(datosParaProcesar, socketPlanifAux);
			pthread_mutex_unlock(&mutex);
		}
		free(lineaLeida);
		fclose(archivo);
		//pthread_mutex_unlock(&mutexProceso);
		//free(textoALoguear);
	}
	free(datosParaProcesar);
	free(mensajeAMemoria);
	free(mensajeDeMemoria);
	close(socketPlanifAux);
}

int main() {
	system("clear");

	// creacion de la instancia de log
	logCpu = log_create("../src/log.txt", "cpu.c", false, LOG_LEVEL_INFO);

	/*tipoConfiguracionCPU* config = malloc(sizeof(tipoConfiguracionCPU));
	 config->ipPlanificador = "127.0.0.1";
	 config->puertoPlanificador = "4143";
	 config->ipMemoria = "127.0.0.1";
	 config->puertoMemoria = "4142";
	 config->cantidadHilos = 1;
	 config->retardo = 1;*/


	pthread_mutex_init(&mutex, NULL);

	config = leerConfiguracion();
	int i;

	//Inicia el Socket para conectarse con la Memoria
	printf("Conectando a la Memoria (%s : %s)... ", config->ipMemoria, config->puertoMemoria);
	client_init(&socketMemoria, config->ipMemoria, config->puertoMemoria);
	printf("OK\n");

	//creacion de hilos
	pthread_t vectorHilos[config->cantidadHilos];
	pthread_attr_t atrib;

	sem_init(&ejecutaInstruccion, 0, 0);
	sem_init(&envioParaMemoria, 0, 1);
	sem_init(&nuevoProceso, 0, config->cantidadHilos - 1);

	pthread_attr_init(&atrib);

	//protocolo_planificador_cpu* parametros = malloc(config->cantidadHilos * sizeof(protocolo_planificador_cpu));
	for (i = 0; i < config->cantidadHilos; i++) {
		//vectorHilos[i].tid= process_get_thread_id();
		//pthread_create(&hilo, &atrib, procesarInstruccion,(void*) parametros);
		vectorHilos[i] = pthread_create(&vectorHilos[i], &atrib,procesarInstruccion, NULL);

	}

	//TODO: AVISAR A PLANIFICADOR CANTIDAD DE HILOS DISPONIBLES
	//TODO: DEFINIR ESTRUCTURA SOCKET-HILO

	/*	int status = 1;

	 while (status != 0) {
	 sem_wait(&nuevoProceso);
	 status = deserializarPlanificador(parametros);
	 logueoRecepcionDePlanif(parametros);
	 terminoPlanificador = false;
	 sem_post(&ejecutaInstruccion);

	 }*/

	for (i = 0; i < config->cantidadHilos; i++) {
		pthread_join(vectorHilos[i], NULL);
	}

	sem_wait(&ejecutaInstruccion);
	printf("Finalizo el planificador...\n");

	//free(parametros);
	close(socketMemoria);
	sem_destroy(&envioParaMemoria);
	sem_destroy(&ejecutaInstruccion);
	sem_destroy(&nuevoProceso);
	pthread_mutex_destroy(&mutex);

	pthread_attr_destroy(&atrib);
	log_info(logCpu, "Cerrada conexion saliente");
	log_destroy(logCpu);
	pthread_exit(0);
	return 0;
}

