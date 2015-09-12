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

//Arma estructura del archivo en swap
tconfig_swap* leerConfiguracion() {
	tconfig_swap* datosSwap = malloc(sizeof(tconfig_swap));
	t_config* config;
	config = config_create("../src/swap.cfg");
	datosSwap->puertoEscucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	datosSwap->nombreSwap = config_get_string_value(config, "NOMBRE_SWAP");
	datosSwap->cantidadPaginas = config_get_int_value(config, "CANTIDAD_PAGINAS");
	datosSwap->tamanioPagina = config_get_int_value(config, "TAMANIO_PAGINA");
	datosSwap->retardo = config_get_int_value(config, "RETARDO_COMPACTATION");
	return datosSwap;
}
