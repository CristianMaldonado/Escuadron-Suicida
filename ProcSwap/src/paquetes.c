/*
 * paquetes.c
 *
 *  Created on: 25/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include "estructuras.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>

void iniciar_archivo_swap(void) {
	tconfig_swap *config_swap = leerConfiguracion();

	FILE* swap = fopen(config_swap->nombreSwap, "w+");

	size_t tamanio_swap = config_swap->tamanioPagina * config_swap->cantidadPaginas;

	// rellenamos en cero (char '/0' es 0)
	int i ; char cero = 0;
	for (i = 0 ; i < tamanio_swap; i++)
		fwrite(&cero, sizeof(cero), 1, swap);

	fclose(swap);
}

void des_serializar_memoria(void* chorro, tprotocolo_memoria_swap *paquete_desde_memoria) {

	memcpy(&(paquete_desde_memoria->codigo_op), chorro ,1);
	memcpy(&(paquete_desde_memoria->pid), chorro + 1, 4);
	memcpy(&(paquete_desde_memoria->cantidad_pagina), chorro + 5, 4);
	memcpy(&(paquete_desde_memoria->tamanio_mensaje), chorro + 9, 4);

	paquete_desde_memoria->mensaje = malloc(paquete_desde_memoria->tamanio_mensaje + 1);
	memcpy(paquete_desde_memoria->mensaje, chorro + 13, paquete_desde_memoria->tamanio_mensaje);
	paquete_desde_memoria->mensaje[paquete_desde_memoria->tamanio_mensaje] = '\0';

}
