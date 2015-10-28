/*
 * config.c
 *
 *  Created on: 12/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include "libSocket.h"
#include <commons/config.h>
#include <commons/log.h>
#include "estructuras.h"
#include "paquetes.h"
#include <commons/string.h>
#include <string.h>




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



void log_inicializar(t_log *log, int pid, int nro_pagina_inicial, int tamanio_pagina, int paginas_asignadas) {

	char * str = malloc(20);
	strcpy(str, "inicializar-> pid: ");
	string_append(&str, string_itoa(pid));
	string_append(&str, ", bytes_inicial: ");
	string_append(&str, string_itoa(nro_pagina_inicial*tamanio_pagina));
	string_append(&str, ", tamanio_en_bytes: ");
	string_append(&str, string_itoa(paginas_asignadas*tamanio_pagina));
	string_append(&str, "\n");

	log_info(log,str);
}

void log_finalizar(t_log *log, int pid, int tamanio_pagina, int paginas_asignadas) {

	char * str = malloc(20);;
	strcpy(str, "finalizado-> pid: ");
	string_append(&str, string_itoa(pid));
	string_append(&str, ", tamanio_en_bytes_liberados: ");
	string_append(&str, string_itoa(paginas_asignadas*tamanio_pagina));
	string_append(&str, "\n");

	log_info(log,str);
}

void log_proc_rechazado(t_log *log, int pid){

	char * str = malloc(30);
	strcpy(str, "proceso rechazado-> pid: ");
	string_append(&str, string_itoa(pid));
	string_append(&str, "\n");

	log_info(log,str);
}

void log_lectura(t_log *log, int pid, int nro_pagina_inicial, int tamanio_pagina, int pagina_a_leer, char* contenido){

	char * str = malloc(20);
	strcpy(str, "lectura-> pid: ");
	string_append(&str, string_itoa(pid));
	string_append(&str, ", bytes_inicial: ");
	string_append(&str, string_itoa((nro_pagina_inicial + pagina_a_leer)*tamanio_pagina));
	string_append(&str, ", tamanio_en_bytes_a_leer: ");
	string_append(&str, string_itoa(tamanio_pagina));
	string_append(&str, ", contenido: ");
	string_append(&str, contenido);
	string_append(&str, "\n");

	log_info(log,str);

}

