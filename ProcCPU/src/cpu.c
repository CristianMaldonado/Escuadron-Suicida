#include <pthread.h>
#include <semaphore.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include "estructuras.h"
#include "funcionesCPU.h"

#define PACKAGESIZE 30


void *procesarInstruccion(void *argumento){
	tParametroHilo* datosParaProcesar;
	datosParaProcesar = (tParametroHilo*)argumento;

	while(1){
		sem_wait(&ejecutaInstruccion); //TODO
		//message=(tMensajeAMemoria*)malloc(sizeof(tMensajeAMemoria));
		//funcion leer() que lea el archivo especificado en la ruta (datosParaProcesar.mensajeAPlanificador.mensaje) return instruccion
		//interpretarInstruccion(instruccion,datosParaProcesar);
		//char* mensajeParaMemoria=malloc();//////
		//enviar(&mensajeParaMemoria);
		//liberar_paquete();
		//free(message);
	}
}


int main() {
	system("clear");

	pthread_t hilo;
	pthread_attr_t attr;
	sem_init(&ejecutaInstruccion,0,0);

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

	// crea hilo para ejecutar comandos del planificador TODO
	// Lo que recibimos del planificador lo enviamos al hilo
	//pthread_create(&hilo, &atrib, procesarInstruccion,(void*) parametros);
	//pthread_attr_init(attr);
	//while(){ aca se produce la magia
	//	revc(socketPlanificador);
	//	cargo estructura para hilo
	//	recv(memoria);
	//	sem_post();
	//}

	/*Pasaje de mensaje*/
	char package[PACKAGESIZE];
	int status = 1;

	while (status != 0) { //TODO volar a la mierda
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
