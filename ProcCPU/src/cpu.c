#include <pthread.h>
#include <semaphore.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include "estructuras.h"
#include "funcionesCPU.h"
#include "serializacion.h"

#define PACKAGESIZE 30


void *procesarInstruccion(void *argumento){
	tMensajeAMemoria* message;
	message = (tMensajeAMemoria*)argumento;

	while(1){
		sem_wait(&ejecutaInstruccion);
		//message=(tMensajeAMemoria*)malloc(sizeof(tMensajeAMemoria));
		//interpretarInstruccion(message);
	}
}
// la funcion que procesa el hilo
/*void* procesarInstruccion(void* arg){

	string_n_split(linea, 3, " ");
	while (1){
		sem_wait(&ejecutaInstruccion);
		swith(instruccion){
			case 1:
				armarPaquete(&paquete, 'i', '\0', pid, linea[1], '\0');
				serializar(&paquete);
				//send(socketMemoria, paquete, sizeof(paquete), 0); paquete es tipo estructura hay que serializar
				break;
			case 2:
				armarPaquete(&paquete, 'l', '\0', pid, linea[1], '\0');
				serializar(&paquete);
				//send(socketMemoria, paquete, sizeof(paquete), 0); paquete es tipo estructura hay que serializar
				break;
			case 3:
				armarPaquete(&paquete, 'e', '\0', pid, linea[1], '\0');
				serializar(&paquete);
				//send(socketMemoria, paquete, sizeof(paquete), 0); paquete es tipo estructura hay que serializar
				break;
			case 4:
				armarPaquete(&paquete, 'i', '\0', pid, linea[1], '\0');
				serializar(&paquete);
				//send(socketMemoria, paquete, sizeof(paquete), 0); paquete es tipo estructura hay que serializar
				break;
			case 5:
				armarPaquete(&paquete, 'f', '\0', pid, 0, '\0');
				serializar(&paquete);
				//send(socketMemoria, paquete, sizeof(paquete), 0); paquete es tipo estructura hay que serializar
				break;
			default:
				error_show("Archivo dañado / linea imposible de interpretar");
				break;
		}

	}
}*/


int main() {
	system("clear");

	pthread_t hilo;
	pthread_attr_t attr;
	int id_instruccion;
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

	// crea hilo para ejecutar comandos del planificador
	//pthread_create(&hilo, &atrib, procesarInstruccion,(void*) parametros);
	//pthread_attr_init(attr);
	//while(){
	//	revc(socketPlanificador);
	//	id_instruccion = interpretarInstruccion(instruccion);
	//}

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
