#include <pthread.h>
#include "libSocket.h"
#include <commons/config.h>
#include <commons/process.h>
#include "estructuras.h"
#include "funcionesCPU.h"
#include <commons/error.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

bool terminoPlanificador;

//TODO estructuras que usa el hilo definirlas aca
void *procesarInstruccion(void *argumento){

	protocolo_planificador_cpu* datosParaProcesar;
	datosParaProcesar = (protocolo_planificador_cpu*)argumento;
	protocolo_cpu_memoria* mensajeAMemoria = malloc(sizeof(protocolo_cpu_memoria));
	protocolo_memoria_cpu* mensajeDeMemoria = malloc(sizeof(protocolo_memoria_cpu));
	int tid = process_get_thread_id();

	//LOGUEO DE CONEXION CON MEMORIA ---------> TODO PREGUNTAR POR LOG_TRACE
	char* log=(char*)malloc(5);//malloc(2)
	strcpy(log,"CPU ");
	string_append(&log,string_itoa(tid));
	//if(socketMemoria == -1){ //POR ESTOS LOGS EL VALGRIND TIRA 57 ERRORES
	//	string_append(&log," falló al conectar con Memoria");
	//}
	//else{
	//	string_append(&log," conectado a la Memoria");
	//}
	//log_info(logCpu, log);
	free(log);//malloc(2)
	while(1){
		sem_wait(&ejecutaInstruccion); //TODO

		if(terminoPlanificador) pthread_exit(0);

		FILE* archivo = fopen(datosParaProcesar->mensaje, "r+");

		if(archivo== NULL) error_show("Error al abrir mCod");

		fseek(archivo, 0, SEEK_END);
		int tamanio = ftell(archivo);
		fseek(archivo, 0, SEEK_SET);
		char* lineaLeida = malloc(tamanio);

		while(!feof(archivo)){ //TODO: Agregar lo del quatum
			char* instruccionLeida = leerInstruccion(&(datosParaProcesar->counterProgram), lineaLeida, archivo,tamanio);

			printf("linea %s\n",instruccionLeida);
			interpretarInstruccion(instruccionLeida, datosParaProcesar,mensajeAMemoria); //arma el paquete para memoria y lo carga en mensajeAMemoria

			enviarAMemoria(mensajeAMemoria);
			printf("pid %d\n",mensajeAMemoria->pid);
			printf("tamanio %d\n",mensajeAMemoria->tamanioMensaje);
			printf("operacion %c\n",mensajeAMemoria->tipoOperacion);
			deserializarMemoria(mensajeDeMemoria);
			printf("pid %d\n",mensajeDeMemoria->pid);
			printf("tamanio %d\n",mensajeDeMemoria->tamanioMensaje);
			printf("operacion %c\n",mensajeDeMemoria->codOperacion);
			 /* switch (mensajeDeMemoria->codOperacion){

			            case 'i': {
			             	 armarPaquetePlanificador(datosParaProcesar, (mensajeDeMemoria->codAux == 'a')?'f' : 'c', 'i',
			                   	                		 mensajeAMemoria->pid,
			                   							 datosParaProcesar->counterProgram,datosParaProcesar->quantum,
														 datosParaProcesar->tamanioMensaje, datosParaProcesar->mensaje);
			                   }break;

			            case 'l': {
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

		}
		free(lineaLeida);
		fclose(archivo);
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

	tipoConfiguracionCPU *config = leerConfiguracion();

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
	//if(socketMemoria == -1){log_info(logCpu, "Fallo al conectar con Memoria");}
	//else{log_info(logCpu, "Conectado a la Memoria");}

	//Hilo
	pthread_t hilo;
	pthread_attr_t atrib;
	sem_init(&ejecutaInstruccion,0,0);
	protocolo_planificador_cpu* parametros = malloc(sizeof(protocolo_planificador_cpu));

	// Lo que recibimos del planificador lo enviamos al hilo
	pthread_attr_init(&atrib);
	pthread_create(&hilo, &atrib, procesarInstruccion,(void*) parametros);

	int status = 1;

	while (status != 0) {
		status = deserializarPlanificador(parametros);
		logueoRecepcionDePlanif(parametros);
		terminoPlanificador = false;
		sem_post(&ejecutaInstruccion);

	}
	free(parametros);

	printf("Finalizo el planificador...\n");

	terminoPlanificador = true;

	pthread_join(hilo,NULL);

	close(socketMemoria);
	close(socketPlanificador);
	log_info(logCpu, "Cerrada conexion saliente");
	log_destroy(logCpu);
	return 0;
}
