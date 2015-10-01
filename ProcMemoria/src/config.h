/*
 * config.h
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

tconfig_memoria* leerConfiguracion();
void inicializar_sockets(int *socketClienteCPU, int *socketClienteSWAP, int *socketServidorCPU, tconfig_memoria *config);
void finalizar_conexiones(int *socketClienteCPU, int *socketClienteSWAP, int *socketServidorCPU, tconfig_memoria *config);
#endif /* SRC_CONFIG_H_ */
