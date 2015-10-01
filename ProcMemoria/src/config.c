/*
 * config.c
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#include <stdlib.h>
#include "estructuras.h"
#include "config.h"

#include <commons/config.h>
//Arma estructura del archivo en memoria
tconfig_memoria* leerConfiguracion() {
	tconfig_memoria* datosMemoria = malloc(sizeof(tconfig_memoria));
	t_config* config;
	config = config_create("../src/memoria.cfg");
	datosMemoria->puertoEscucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	datosMemoria->ipSwap = config_get_string_value(config, "IP_SWAP");
	datosMemoria->puertoSwap = config_get_string_value(config, "PUERTO_SWAP");
	datosMemoria->maximoMarcosPorProceso = config_get_int_value(config,"MAXIMO_MARCOS_POR_PROCESO");
	datosMemoria->cantidadMarcos = config_get_int_value(config,"CANTIDAD_MARCOS");
	datosMemoria->tamanioMarco = config_get_int_value(config, "TAMANIO_MARCO");
	datosMemoria->entradasTLB = config_get_int_value(config, "ENTRADAS_TLB");
	datosMemoria->habilitadaTLB = config_get_string_value(config,"TLB_HABILITADA")[0];
	datosMemoria->retardoMemoria = config_get_int_value(config,"RETARDO_MEMORIA");
	return datosMemoria;
}

void inicializar_sockets(int *socketClienteCPU, int *socketClienteSWAP, int *socketServidorCPU, tconfig_memoria *config) {

	printf("Conectando al SWAP (%s : %s)... ", config->ipSwap, config->puertoEscucha);
	client_init(&socketClienteSWAP, config->ipSwap, config->puertoEscucha);
	printf("OK\n");

	//Definimos datos Server
	server_init(&socketServidorCPU, "4142");
	printf("Memoria lista...\n");

	server_acept(socketServidorCPU, &socketClienteCPU);
	printf("CPU aceptado...\n");
}

void finalizar_conexiones(int *socketClienteCPU, int *socketClienteSWAP, int *socketServidorCPU, tconfig_memoria *config) {
	close(socketClienteSWAP);
	close(socketClienteCPU);
	close(socketServidorCPU);
}
