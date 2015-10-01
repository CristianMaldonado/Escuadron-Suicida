/*
 * config.h
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

tconfig_swap* leerConfiguracion();
void finalizar(int *server_socket, int *socket_memoria, FILE *swap, tconfig_swap *config_swap);

#endif /* SRC_CONFIG_H_ */
