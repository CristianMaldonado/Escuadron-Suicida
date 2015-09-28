#include <pthread.h>
#include <semaphore.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include <commons/process.h>
#include "estructuras.h"
#include "funcionesCPU.h"
#include <commons/error.h>
#include <string.h>


void *procesarInstruccion(void *argumento){
	tParametroHilo* datosParaProcesar;
	datosParaProcesar = (tParametroHilo*)argumento;
	int tid = process_get_thread_id();


	//LOGUEO DE CONEXION CON MEMORIA ---------> TODO PREGUNTAR POR LOG_TRACE
	char* log=(char*)malloc(4);//malloc(2)----> NO PRESTES ATENCION A ESTOS MALLOC CON NUMERO ES PARA GUIARME SI A UN MALLOC LE TIRO UN FREE
	strcpy(log,"CPU ");
	string_append(&log,string_itoa(tid));
	if(datosParaProcesar->socketMemoria == -1){
		string_append(&log," falló al conectar con Memoria");
		log_info(logCpu, log);
	}
	else{
		string_append(&log," conectado a la Memoria");
		log_info(logCpu, log);
	}
	free(log);//malloc(2)----> NO PRESTES ATENCION A ESTOS MALLOC CON NUMERO ES PARA GUIARME SI A UN MALLOC LE TIRO UN FREE

	while(1){
		sem_wait(&ejecutaInstruccion); //TODO
		char* instruccionLeida;
		strcpy(instruccionLeida,string_new());
		FILE* archivo = fopen(datosParaProcesar->mensajeAPlanificador->mensaje, "r");
		if (archivo == NULL) {
			error_show("Error al abrir mCod");
		}

		while(!feof(archivo)){ //TODO: Agregar lo del quatum

		strcpy(instruccionLeida, leerInstruccion(&(datosParaProcesar->mensajeAPlanificador->counterProgram), archivo));
		interpretarInstruccion(instruccionLeida, datosParaProcesar);
        send(datosParaProcesar->socketMemoria, instruccionLeida, strlen(instruccionLeida)+1, 0);
		//controlar en while con eof o quantum
		//funcion leer() que lea el archivo especificado en la ruta (datosParaProcesar.mensajeAPlanificador.mensaje)
		//return instruccion
		//interpretarInstruccion(instruccion,datosParaProcesar);
		//char* mensajeParaMemoria=malloc();//////
		//enviar(&mensajeParaMemoria);
		//free(message);

		fclose(archivo);
		free(instruccionLeida);
		}
	}
}


int main() {
	system("clear");

	// creacion de la instancia de log
	logCpu = log_create("../src/log.txt", "cpu.c", false,LOG_LEVEL_INFO);

	tipoConfiguracionCPU *config = leerConfiguracion();

	/*Inicia el Socket para conectarse con el Planificador*/
	int socketPlanificador;
	printf("Conectando al Planificador (%s : %s)... ", config->ipPlanificador,
			config->puertoPlanificador);
	client_init(&socketPlanificador, config->ipPlanificador,
			config->puertoPlanificador);
	printf("OK\n");

	//loguea conexion con Planificador
	if(socketPlanificador == -1){log_info(logCpu, "Fallo al conectar con Planificador");}
	else{log_info(logCpu, "Conectado al Planificador");}

	/*Inicia el Socket para conectarse con la Memoria*/
	int socketMemoria;
	printf("Conectando a la Memoria (%s : %s)... ", config->ipMemoria,
			config->puertoMemoria);
	client_init(&socketMemoria, config->ipMemoria, config->puertoMemoria); printf("OK\n");

	//loguea conexion con Memoria
	//if(socketMemoria == -1){log_info(logCpu, "Fallo al conectar con Memoria");}
	//else{log_info(logCpu, "Conectado a la Memoria");}

	//Hilo
	pthread_t hilo;
	pthread_attr_t atrib;
	sem_init(&ejecutaInstruccion,0,0);
	tParametroHilo* parametros;

	// Lo que recibimos del planificador lo enviamos al hilo
	pthread_attr_init(&atrib);
	pthread_create(&hilo, &atrib, procesarInstruccion,(void*) parametros);

	protocolo_planificador_cpu package;
	int status = 1;

	while (status != 0) {
		//status = recv planificador TODO
		//status = recivir_deserializar(&package,socketPlanificador);
		logueoRecepcionDePlanif(&package);
		cargarParametrosHilo(socketPlanificador,socketMemoria,&package,parametros);//puntero al paquqete deserializado?
		sem_post(&ejecutaInstruccion);
	}

	printf("Finalizo el planificador...\n");

	close(socketMemoria);
	close(socketPlanificador);
	log_info(logCpu, "Cerrada conexion saliente");
	return 0;
}
