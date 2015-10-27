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

void *procesarInstruccion(void *argumento){

	protocolo_planificador_cpu* datosParaProcesar;
	datosParaProcesar = (protocolo_planificador_cpu*)argumento;
	protocolo_cpu_memoria* mensajeAMemoria = malloc(sizeof(protocolo_cpu_memoria));
	protocolo_memoria_cpu* mensajeDeMemoria = malloc(sizeof(protocolo_memoria_cpu));
	int tid = process_get_thread_id();

	//LOGUEO DE CONEXION CON MEMORIA

	if(socketMemoria == -1){
		log_info(logCpu, "CPU %d fallo al conectar con Memoria", tid);
	}
	else{
		log_info(logCpu, "CPU %d se conecto con Memoria", tid);
	}

	while(1){
		sem_wait(&ejecutaInstruccion); //TODO

		if(terminoPlanificador) pthread_exit(0);

		FILE* archivo = fopen(datosParaProcesar->mensaje, "r+");

		if(archivo== NULL) error_show("Error al abrir mCod");

		fseek(archivo, 0, SEEK_END);
		int tamanio = ftell(archivo);
		fseek(archivo, 0, SEEK_SET);
		char* lineaLeida = malloc(tamanio);
		int quantum = 0;

		while((!feof(archivo) && (quantum<= datosParaProcesar->quantum || datosParaProcesar->quantum == 0))){ //TODO: Agregar lo del quatum
			//calcularTamanioDeLinea(archivo,&tamanio);
			char* instruccionLeida = leerInstruccion(&(datosParaProcesar->counterProgram), lineaLeida, archivo,tamanio);

			printf("linea %s\n",instruccionLeida);
			interpretarInstruccion(instruccionLeida, datosParaProcesar,mensajeAMemoria); //arma el paquete para memoria y lo carga en mensajeAMemoria
			if(datosParaProcesar->tipoOperacion == 'E') break;


			enviarAMemoria(mensajeAMemoria);
			printf("pid %d\n",mensajeAMemoria->pid);
			printf("tamanio %d\n",mensajeAMemoria->tamanioMensaje);
			printf("operacion %c\n",mensajeAMemoria->tipoOperacion);
			deserializarMemoria(mensajeDeMemoria);
			printf("pid %d\n",mensajeDeMemoria->pid);
			printf("tamanio %d\n",mensajeDeMemoria->tamanioMensaje);
			printf("operacion %c\n",mensajeDeMemoria->codOperacion);
			printf("cod aux %c\n",mensajeDeMemoria->codAux);
			if(mensajeDeMemoria->codAux == 'a' && mensajeDeMemoria->codOperacion == 'i') break;
			 /* switch (mensajeDeMemoria->codOperacion){

			            case 'i': {
			             	 armarPaquetePlanificador(datosParaProcesar, (mensajeDeMemoria->codAux == 'a')?'f' : 'c', 'i',
			                   	                		 mensajeAMemoria->pid,
			                   							 datosParaProcesar->counterProgram,datosParaProcesar->quantum,
														 datosParaProcesar->tamanioMensaje, datosParaProcesar->mensaje);
			                   }break;

			            case 'l','e': {
			            	 armarPaquetePlanificador(datosParaProcesar, (mensajeDeMemoria->codAux == 'a')?'f' : 'c', 'l',
			                  	                		 mensajeAMemoria->pid, datosParaProcesar->estado,
			                  							 datosParaProcesar->counterProgram,datosParaProcesar->quantum,
														 datosParaProcesar->tamanioMensaje,
														 datosParaProcesar->mensaje);
			            }break;
					case 'f':{
			        	 armarPaquetePlanificador(datosParaProcesar, (mensajeDeMemoria->codAux == 'a')?'f' : 'f', 'i',
			              	                		 datosParaProcesar->pid, datosParaProcesar->estado,
			              							 datosParaProcesar->counterProgram,datosParaProcesar->quantum,
													 datosParaProcesar->tamanioMensaje,datosParaProcesar->mensaje);
					}break;


				}*/
			  //enviarAPlanificador(datosParaProcesar);

           loguearEstadoMemoria(mensajeDeMemoria, instruccionLeida);
           sleep(config->retardo); //ver si hay q sincronizar el config (?
           quantum++;
		}
		if(datosParaProcesar->quantum !=0){
			actualizarOperacionPaquetePlanificador(datosParaProcesar,'q');
			enviarAPlanificador(datosParaProcesar);
		}
		free(lineaLeida);
		fclose(archivo);
		sem_post(&nuevoProceso);
	}
	free(mensajeAMemoria);
	free(mensajeDeMemoria);
}


int main() {
	system("clear");

	// creacion de la instancia de log
	logCpu = log_create("../src/log.txt", "cpu.c", false,LOG_LEVEL_INFO);

	/*tipoConfiguracionCPU* config = malloc(sizeof(tipoConfiguracionCPU));
	config->ipPlanificador = "127.0.0.1";
	config->puertoPlanificador = "4143";
	config->ipMemoria = "127.0.0.1";
	config->puertoMemoria = "4142";
	config->cantidadHilos = 4;
	config->retardo = 2;*/

	config = leerConfiguracion();
	 int i;
	//Inicia el Socket para conectarse con el Planificador/
	printf("Conectando al Planificador (%s : %s)... ", config->ipPlanificador,
			config->puertoPlanificador);
	//for(i=0;i<=config->cantidadHilos;i++){
	client_init(&socketPlanificador, config->ipPlanificador,
			config->puertoPlanificador);
	printf("OK\n");
	//vectorHilos[i].socket = socketPlanificador;

	//loguea conexion con Planificador
	if(socketPlanificador == -1)
	log_info(logCpu, "Fallo al conectar con Planificador");
	else log_info(logCpu, "Conectado al Planificador");

	//Inicia el Socket para conectarse con la Memoria
	printf("Conectando a la Memoria (%s : %s)... ", config->ipMemoria,
			config->puertoMemoria);
	client_init(&socketMemoria, config->ipMemoria, config->puertoMemoria); printf("OK\n");

    pthread_t vectorHilos[config->cantidadHilos];

	//Hilo
	pthread_t hilo;
	pthread_attr_t atrib;
	sem_init(&ejecutaInstruccion,0,0);
	sem_init(&nuevoProceso,0,1);
	pthread_attr_init(&atrib);

	protocolo_planificador_cpu* parametros = malloc(config->cantidadHilos * sizeof(protocolo_planificador_cpu));
	//for (i=0; i<= config->cantidadHilos; i++){
     //vectorHilos[i].tid= process_get_thread_id();

	// Lo que recibimos del planificador lo enviamos al hilo
	pthread_create(&hilo, &atrib, procesarInstruccion,(void*) parametros);
	//pthread_create(&vectorHilos[i], &atrib, procesarInstruccion,(void*) parametros);
	//vectorHilos[i] = process_get_thread_id();
	//}

	//TODO: AVISAR A PLANIFICADOR CANTIDAD DE HILOS DISPONIBLES
	//TODO: DEFINIR ESTRUCTURA SOCKET-HILO

	int status = 1;

	while (status != 0) {
		sem_wait(&nuevoProceso);
		status = deserializarPlanificador(parametros);
		logueoRecepcionDePlanif(parametros);
		terminoPlanificador = false;
		sem_post(&ejecutaInstruccion);

	}
	free(parametros);

	printf("Finalizo el planificador...\n");

	terminoPlanificador = true;

	pthread_join(hilo,NULL);
	/*for(i=0;i<=config->cantidadHilos;i++){
		pthread_join(vectorHilos[i],NULL);
	}*/


	close(socketMemoria);
	close(socketPlanificador);
	sem_destroy(&nuevoProceso);
	sem_destroy(&ejecutaInstruccion);
	log_info(logCpu, "Cerrada conexion saliente");
	log_destroy(logCpu);
	return 0;
}




