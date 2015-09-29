/*
 * estructuras_swap.c
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include "estructuras.h"
#include "paquetes.h"
#include <string.h>

void lista_vacia_compactada(t_list **lista_vacia, FILE **swap, int tamanio_pagina ,int total_de_paginas) {
	list_destroy_and_destroy_elements(*lista_vacia, free);
	int comienzo = (int)ftell(*swap) / tamanio_pagina;
	tlista_vacio *vacio = malloc(sizeof(tlista_vacio));
	vacio->comienzo = comienzo;
	vacio->paginas_vacias = total_de_paginas - vacio->comienzo;
	*lista_vacia = malloc(sizeof(tlista_vacio));
	list_add(*lista_vacia, vacio);
}


void reinicar_archivo_swap(FILE **swap, t_list **lista_ocupada) {
	fclose(*swap);
	*swap = iniciar_archivo_swap();
	list_destroy_and_destroy_elements(*lista_ocupada, free);
}

t_list *pasar_ocupada_a_lista_auxiliar(FILE **swap, t_list **lista_ocupada, int tamanio_pagina) {
	t_list *lista_aux = list_create();
	tlista_ocupado *elem = malloc(sizeof(tlista_ocupado));
	while (!list_is_empty(*lista_ocupada)) {
		elem = list_remove(*lista_ocupada, 0);
		tdatos_paginas *data = malloc(sizeof(tdatos_paginas));
		data->pid = elem->pid;
		data->tamanio = elem->paginas_ocupadas*tamanio_pagina;//en bytes
		data->buffer = (char*)malloc(data->tamanio);// puede haber igual o menor del tamanio
		// leemos los datos
		fseek(*swap, elem->comienzo*tamanio_pagina, SEEK_SET);
		fread(data->buffer, sizeof(char), tamanio_pagina*elem->paginas_ocupadas , *swap); // lee y guarda en buffer pero tiene ceros al final
		list_add(lista_aux,data);
	}
	free(elem);
	return lista_aux;
}

int get_comienzo_espacio_asignado(t_list * lista_ocupado, int pid) {
	int i;
	for (i = 0; i < list_size(lista_ocupado); i++){
		tlista_ocupado * ocupado = malloc(sizeof(tlista_ocupado));
		ocupado = list_get(lista_ocupado, i);

		if (ocupado->pid == pid) {
			int comienzo = ocupado->comienzo;
			free(ocupado);
			return comienzo;
		}
		free(ocupado);
	}
	return -1; // si no encuentra el pid en la lista
}


int dame_si_hay_espacio(t_list* lista_vacia, int paginas_pedidas) {
	int i;
	for (i = 0; i < list_size(lista_vacia); i++) {
		tlista_vacio *aux = malloc(sizeof(tlista_vacio));
		aux = list_get(lista_vacia, i);
		if (aux->paginas_vacias >= paginas_pedidas) {
			//actualizar el hueco vacio
			list_remove(lista_vacia, i);
			//si sigue existiendo un hueco osea != 0
			if (aux->paginas_vacias - paginas_pedidas != 0) {
				aux->comienzo += paginas_pedidas;
				aux->paginas_vacias -= paginas_pedidas;
				list_add(lista_vacia, aux);
			}
			int comienzo = aux->comienzo;
			free(aux);
			return comienzo;
		}
		free(aux);
	}
	return -1;
}


int espacio_total_disponible(t_list* lista_vacia) {
	int tamanio = 0, i;
	tlista_vacio *aux = malloc(sizeof(tlista_vacio));
	for (i = 0; i < list_size(lista_vacia); i++) {
		aux = list_get(lista_vacia, i);
		tamanio += aux->paginas_vacias;
	}
	free(aux);
	return tamanio;
}
