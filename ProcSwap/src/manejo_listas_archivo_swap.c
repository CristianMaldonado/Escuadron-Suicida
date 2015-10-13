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






void reinicar_archivo_swap(FILE **swap, t_list **lista_ocupada) {
	fclose(*swap);
	*swap = iniciar_archivo_swap();
	list_destroy_and_destroy_elements(*lista_ocupada, free);
	*lista_ocupada = malloc(sizeof(tlista_ocupado));
}

t_list *pasar_ocupada_a_lista_auxiliar(FILE **swap, t_list **lista_ocupada, int tamanio_pagina) {
	t_list *lista_aux = list_create();
	tlista_ocupado *elem;
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
		free(elem);
	}
	return lista_aux;
}

int get_comienzo_espacio_asignado(t_list * lista_ocupado, int pid) {
	int i;
	for (i = 0; i < list_size(lista_ocupado); i++){
		tlista_ocupado * ocupado = malloc(sizeof(tlista_ocupado));
		ocupado = list_get(lista_ocupado, i);

		if (ocupado->pid == pid) {
			int comienzo = ocupado->comienzo;
			return comienzo;
		}
	}
	return -1; // si no encuentra el pid en la lista
}

// 1: hay espacio ; 2: no hay espacio
int dame_si_hay_espacio(t_list** lista_vacia, int paginas_pedidas, int* comienzo) {
	int i;
	tlista_vacio *aux;
	int flag;
	for (i = 0; i < list_size(*lista_vacia); i++) {
		aux = list_get(*lista_vacia, i);
		*comienzo = aux->comienzo;
		if (aux->paginas_vacias >= paginas_pedidas) {
			flag = 0;
			//actualizar el hueco vacio
			//si sigue existiendo un hueco osea != 0
			if (aux->paginas_vacias - paginas_pedidas != 0) {
				*comienzo = aux->comienzo;
				tlista_vacio *plistV = malloc(sizeof(tlista_vacio));
				plistV->comienzo = paginas_pedidas + aux->comienzo;
				plistV->paginas_vacias = aux->paginas_vacias - paginas_pedidas;

				free(list_remove(*lista_vacia, i));
				list_add(*lista_vacia, plistV);
			}
			else
				free(list_remove(*lista_vacia, i));

			return 1;
		}
		else
			flag = 1;
	}
	/*
	if(flag) {
		*comienzo = aux->comienzo;
		return 0; }*/
	return  0;//aux->comienzo;
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

int lista_vacia_compactada(t_list **lista_vacia, FILE **swap, int tamanio_pagina ,int total_de_paginas) {
	list_destroy_and_destroy_elements(*lista_vacia, free);
	int comienzo = (int)ftell(*swap) / tamanio_pagina;
	tlista_vacio *vacio = malloc(sizeof(tlista_vacio));
	vacio->comienzo = comienzo;
	vacio->paginas_vacias = total_de_paginas - vacio->comienzo;
	*lista_vacia = list_create();
	list_add(*lista_vacia, vacio);
	return vacio->comienzo;
}


int compactar_swap(FILE ** swap, t_list** lista_vacia, t_list** lista_ocupada,int tamanio_pagina, int total_de_paginas) {
	t_list *lista_aux = pasar_ocupada_a_lista_auxiliar(swap, lista_ocupada, tamanio_pagina);
	reinicar_archivo_swap(swap, lista_ocupada);

	int cont_pagina = 0;
	while (!list_is_empty(lista_aux)) {
		tdatos_paginas *elem = malloc(sizeof(tdatos_paginas));
		elem = list_remove(lista_aux, 0);
		fwrite(elem->buffer, 1,elem->tamanio, *swap);
		//actualizamos lista ocupada
		tlista_ocupado *elem_ocupada = malloc(sizeof (tlista_ocupado));
		elem_ocupada->pid = elem->pid;
		elem_ocupada->comienzo = cont_pagina;
		elem_ocupada->paginas_ocupadas = elem->tamanio/tamanio_pagina;
		cont_pagina += elem_ocupada->paginas_ocupadas;
		list_add(*lista_ocupada, elem_ocupada);
		free(elem_ocupada);
	}
	//list_destroy_and_destroy_elements(lista_aux, free);

	return lista_vacia_compactada(lista_vacia, swap, tamanio_pagina ,total_de_paginas);
}


bool comparacion(void * a, void * b) {
	return ((tlista_vacio*)a)->comienzo < ((tlista_vacio*)b)->comienzo;
}

void posicion_ultimo_elemento_contiguo(t_list **lista_vacia, tlista_vacio* elem, int* posicion_final) {

	tlista_vacio * aux = list_get(*lista_vacia, 0);

	if (elem->comienzo + elem->paginas_vacias == aux->comienzo) {
		tlista_vacio *copia = malloc(sizeof(tlista_vacio));
		*copia = *aux;
		*posicion_final = copia->comienzo + copia->paginas_vacias;
		free(list_remove(*lista_vacia, 0));
		if(!list_is_empty(*lista_vacia))
			posicion_ultimo_elemento_contiguo(lista_vacia, copia, posicion_final);
	}
}

void arreglame_la_lista_vacia_che(t_list ** lista_vacia) {
	//if(list_size(*lista_vacia) > 1)
	list_sort(*lista_vacia, comparacion);

	t_list* list_aux = list_create();

	while(!list_is_empty(*lista_vacia)) {
		tlista_vacio *elem_uno = list_remove(*lista_vacia, 0);
		int pos_final = elem_uno->paginas_vacias;

		if(!list_is_empty(*lista_vacia))
			posicion_ultimo_elemento_contiguo(lista_vacia, elem_uno, &pos_final);

		tlista_vacio *elem_bloque_vacio = malloc(sizeof(tlista_vacio));
		elem_bloque_vacio->comienzo = elem_uno->comienzo;
		elem_bloque_vacio->paginas_vacias = pos_final;
		list_add(list_aux, elem_bloque_vacio);

	}
	//list_destroy_and_destroy_elements(*lista_vacia, free);

	*lista_vacia = list_aux;
}











