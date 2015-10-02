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
#include <stdio.h>

//TODO estructuras que usa el hilo definirlas aca
void *procesarInstruccion(void *argumento){
	tParametroHilo* datosParaProcesar;
	datosParaProcesar = (tParametroHilo*)argumento;
	datosParaProcesar->mensajeAMemoria = malloc(sizeof(protocolo_cpu_memoria));
	int tid = process_get_thread_id();

	printf("bandera 0");
	//LOGUEO DE CONEXION CON MEMORIA ---------> TODO PREGUNTAR POR LOG_TRACE
	char* log=(char*)malloc(5);//malloc(2)
	strcpy(log,"CPU ");
	string_append(&log,string_itoa(tid));
	if(socketMemoria == -1){
		string_append(&log," falló al conectar con Memoria");
		//log_info(logCpu, log);
	}
	else{
		string_append(&log," conectado a la Memoria");
		//log_info(logCpu, log);
	}
	free(log);//malloc(2)

	while(1){
		sem_wait(&ejecutaInstruccion); //TODO

		FILE* archivo = fopen(datosParaProcesar->mensajeAPlanificador->mensaje, "r");

		if(archivo== NULL) error_show("Error al abrir mCod");
		fseek(archivo, 0, SEEK_END);
		int tamanio = ftell(archivo);
		fseek(archivo, 0, SEEK_SET);
		char* lineaLeida = malloc(tamanio);

		while(!feof(archivo)){ //TODO: Agregar lo del quatum
		char* instruccionLeida = leerInstruccion(&(datosParaProcesar->mensajeAPlanificador->counterProgram), lineaLeida, archivo,tamanio) ;

		//strcpy(instruccionLeida, leerInstruccion(&(datosParaProcesar->mensajeAPlanificador->counterProgram), lineaLeida, archivo,tamanio));

			interpretarInstruccion(instruccionLeida, datosParaProcesar);

			enviarAMemoria(datosParaProcesar->mensajeAMemoria);

            deserializarMemoria(datosParaProcesar->mensajeDeMemoria);

            switch (datosParaProcesar->mensajeDeMemoria->codOperacion){

            case 'i': {


             	 armarPaquetePlanificador(datosParaProcesar->mensajeAPlanificador, (datosParaProcesar->mensajeDeMemoria->codAux == 'a')?'f' : 'c', 'i',
                   	                		 datosParaProcesar->mensajeAMemoria->pid, datosParaProcesar->mensajeAPlanificador->estado,
                   							 datosParaProcesar->mensajeAPlanificador->counterProgram,datosParaProcesar->mensajeAPlanificador->quantum,
											 datosParaProcesar->mensajeAPlanificador->tamanioMensaje,
											 datosParaProcesar->mensajeAPlanificador->mensaje);
                   }

             break;

            case 'l':  {

            	 armarPaquetePlanificador(datosParaProcesar->mensajeAPlanificador, (datosParaProcesar->mensajeDeMemoria->codAux == 'a')?'f' : 'c', 'l',
                  	                		 datosParaProcesar->mensajeAMemoria->pid, datosParaProcesar->mensajeAPlanificador->estado,
                  							 datosParaProcesar->mensajeAPlanificador->counterProgram,datosParaProcesar->mensajeAPlanificador->quantum,
											 datosParaProcesar->mensajeAPlanificador->tamanioMensaje,
											 datosParaProcesar->mensajeAPlanificador->mensaje);

            } break;

		case 'f':
		{
        	 armarPaquetePlanificador(datosParaProcesar->mensajeAPlanificador, (datosParaProcesar->mensajeDeMemoria->codAux == 'a')?'f' : 'f', 'i',
              	                		 datosParaProcesar->mensajeAMemoria->pid, datosParaProcesar->mensajeAPlanificador->estado,
              							 datosParaProcesar->mensajeAPlanificador->counterProgram,datosParaProcesar->mensajeAPlanificador->quantum,
										 datosParaProcesar->mensajeAPlanificador->tamanioMensaje,
										 datosParaProcesar->mensajeAPlanificador->mensaje);

		} break;


		}
		//send(datosParaProcesar->socketMemoria, datosParaProcesar->mensajeAMemoria, sizeof(datosParaProcesar->mensajeAMemoria), 0);
          // deserializarMemoria(datosParaProcesar->mensajeDeMemoria, datosParaProcesar->socketMemoria);
           //loguearEstadoMemoria(datosParaProcesar->mensajeDeMemoria, instruccionLeida);
		//controlar en while con eof o quantum
		//funcion leer() que lea el archivo especificado en la ruta (datosParaProcesar.mensajeAPlanificador.mensaje)
		//return instruccion
		//interpretarInstruccion(instruccion,datosParaProcesar);
		//char* mensajeParaMemoria=malloc();//////
		//enviar(&mensajeParaMemoria);
		//free(message);
       // free(instruccionLeida);
		}
		free(lineaLeida);
		fclose(archivo);
	}
	free(datosParaProcesar);
}


int main() {
	system("clear");

	// creacion de la instancia de log
	logCpu = log_create("../src/log.txt", "cpu.c", false,LOG_LEVEL_INFO);

	tipoConfiguracionCPU* config = malloc(sizeof(tipoConfiguracionCPU));
	config->ipPlanificador = "127.0.0.1";
	config->puertoPlanificador = "4143";
	config->ipMemoria = "127.0.0.1";
	config->puertoMemoria = "4142";
	config->cantidadHilos = 4;
	config->retardo = 2;

	//tipoConfiguracionCPU *config = leerConfiguracion();

	//Inicia el Socket para conectarse con el Planificador/


	printf("Conectando al Planificador (%s : %s)... ", config->ipPlanificador,
			config->puertoPlanificador);
	client_init(&socketPlanificador, config->ipPlanificador,
			config->puertoPlanificador);
	printf("OK\n");

	//loguea conexion con Planificador
	if(socketPlanificador == -1)
	log_info(logCpu, "Fallo al conectar con Planificador");
	//else log_info(logCpu, "Conectado al Planificador");

	//Inicia el Socket para conectarse con la Memoria

	printf("Conectando a la Memoria (%s : %s)... ", config->ipMemoria,
			config->puertoMemoria);
	client_init(&socketMemoria, config->ipMemoria, config->puertoMemoria); printf("OK\n");

	//loguea conexion con Memoria
	if(socketMemoria == -1){log_info(logCpu, "Fallo al conectar con Memoria");}
	//else{log_info(logCpu, "Conectado a la Memoria");}


	//Hilo
	pthread_t hilo;
	pthread_attr_t atrib;
	sem_init(&ejecutaInstruccion,0,0);
	tParametroHilo* parametros = malloc(sizeof(tParametroHilo));

	// Lo que recibimos del planificador lo enviamos al hilo
	pthread_attr_init(&atrib);
	pthread_create(&hilo, &atrib, procesarInstruccion,(void*) parametros);

	protocolo_planificador_cpu* package= malloc(sizeof(protocolo_planificador_cpu));
	int status = 1;

	while (status != 0) {


		status = deserializarPlanificador(package);
		//crearMockitoPlanif(package);
		//logueoRecepcionDePlanif(package);
		cargarParametrosHilo(package,parametros);
		sem_post(&ejecutaInstruccion);

	}
	free(package);
	free(parametros);
    free(config);
	printf("Finalizo el planificador...\n");

	close(socketMemoria);
	close(socketPlanificador);
	log_info(logCpu, "Cerrada conexion saliente");
	log_destroy(logCpu);
	return 0;
}
