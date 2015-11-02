#include <stdlib.h>
#include "estructuras.h"
#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdbool.h>
#include <string.h>
#include "paquetes.h"
#include <sys/types.h>
#include <sys/socket.h>

char * crear_memoria(int cantidad_marcos, int tamanio_marcos) {
	return malloc(cantidad_marcos * tamanio_marcos);
}

tabla_paginas * inicializar_tabla_de_paginas(int cantidad_maxima_marcos_por_proceso, int pid) {
	tabla_paginas * tabla = malloc(sizeof(tabla_paginas));
	tabla->list_pagina_direccion = list_create();
	tabla->pid = pid;
	int i;
	for(i = 0 ; i < cantidad_maxima_marcos_por_proceso ; i++) {
		pagina_direccion *pagina = malloc(sizeof(pagina_direccion));
		pagina->en_uso = false;
		pagina->fue_modificado = false;
		list_add(tabla->list_pagina_direccion, pagina);
	}
	return tabla;
}

void eliminar_tabla_de_proceso(int pid, t_list ** lista_tabla_de_paginas) {
	int i;
	int count = list_size(*lista_tabla_de_paginas);
	for(i = 0 ; i < count ; i++) {
		tabla_paginas *tabla = list_get(*lista_tabla_de_paginas, i);
		if(tabla->pid == pid)
			free(list_remove(*lista_tabla_de_paginas, i));
	}
}

tabla_paginas * dame_la_tabla_de_paginas(int pid, t_list ** lista_tabla_de_paginas) {
	int i;
	for(i = 0 ; i < list_size(*lista_tabla_de_paginas) ; i++) {
		tabla_paginas *tabla = list_get(*lista_tabla_de_paginas, i);
		if(tabla->pid == pid)
			return tabla;
	}
	return 0;
}

int dame_la_direccion_de_la_pagina(tabla_paginas *tabla, int pagina) {
	int i;
	for(i = 0 ; i < list_size(tabla->list_pagina_direccion) ; i++) {
		pagina_direccion *pagina_aux = list_get(tabla->list_pagina_direccion, i);
		if(pagina_aux->en_uso && pagina_aux->nro_pagina == pagina)
			return pagina_aux->nro_marco;
	}
	return -1;
}

void poneme_en_modificado_la_entrada(tabla_paginas *tabla, int pagina) {
	int i;
	for(i = 0 ; i < list_size(tabla->list_pagina_direccion) ; i++) {
		pagina_direccion *pagina_aux = list_get(tabla->list_pagina_direccion, i);
		if(pagina_aux->en_uso && pagina_aux->nro_pagina == pagina) {
			pagina_aux->fue_modificado = true;
			break;
		}
	}
}

bool estan_los_frames_ocupados(t_list *tabla_paginas) {
	int i = 0;
	pagina_direccion *pagina = list_get(tabla_paginas, 0);
	while(pagina) {
		if(!pagina->en_uso)
			return false;
		i++;
		pagina = list_get(tabla_paginas, i);
	}
	return true;
}

int dame_un_marco_libre(t_list *lista_tabla_de_paginas, int cantidad_marcos) {
	int i, j, k;
	for(i = 0 ; i < cantidad_marcos ; i++) {
		bool libre = true;
		for(j = 0 ; j < list_size(lista_tabla_de_paginas) ; j++) {
			bool ocupado = false;
			tabla_paginas *tabla = list_get(lista_tabla_de_paginas, j);
			for(k = 0 ; k < list_size(tabla->list_pagina_direccion) ; k++) {
				pagina_direccion *pagina = list_get(tabla->list_pagina_direccion, k);
				if(pagina->nro_marco == i && pagina->en_uso) {
					ocupado = true;
					libre = false;
					break;
				}
			}
			if (ocupado)
				break;
		}
		if(libre)
			return i;
	}
	return -1;
}

char * dame_mensaje_de_memoria(char **memoria, int nro_marco, int tamanio_marco) {
	char *mensaje_memoria = *memoria + nro_marco * tamanio_marco;
	char *mensaje = malloc(tamanio_marco + 1);
	memcpy(mensaje, mensaje_memoria, tamanio_marco);
	mensaje[tamanio_marco] = '\0';
	return mensaje;
}

void avisar_a_cpu(char cod_op, char cod_aux, int pid, int paginas, char *mensaje, int socket_cli_cpu) {
	tprotocolo_memoria_cpu memoria_cpu;
	armar_estructura_protocolo_a_cpu(&memoria_cpu, cod_op, cod_aux, pid, paginas, mensaje);
	void * buffer = serializar_a_cpu(&memoria_cpu);
	send(socket_cli_cpu, buffer, strlen(memoria_cpu.mensaje) + 15, 0);
	free(buffer);
}

t_list * inicializar_tlb(int nro_entradas) {
	t_list * tlb = list_create();
	int i;
	for(i = 0 ; i < nro_entradas ; i++) {
		cache_13 *entrada = malloc(sizeof(cache_13));
		entrada->esta_en_uso = false;
		list_add(tlb, entrada);
	}
	return tlb;
}

int dame_la_direccion_posta_de_la_pagina_en_la_tlb(t_list ** tlb, int pid, int nro_pagina) {
	int i;
	for(i = 0; i < list_size(*tlb); i++) {
		cache_13 * aux = list_get(*tlb, i);
		if(aux->pid == pid && aux->nro_pagina == nro_pagina)
			return aux->direccion_posta;
	}
	return -1;
}

char actualizame_la_tlb(t_list ** tlb, int pid, int direccion_posta, int nro_pagina) {
	int i;
	for(i = 0; i< list_size(*tlb); i++) {
		cache_13 * aux = list_get(*tlb, i);
		if(!aux->esta_en_uso) {
			aux->direccion_posta = direccion_posta;
			aux->esta_en_uso = true;
			aux->pid = pid;
			aux->nro_pagina = nro_pagina;
			return 'e'; // e = encontro una entrada
		}
	}
	// esta lleno, sacar por fifo
	free(list_remove(*tlb, 0));
	cache_13 * nueva_entrada = malloc(sizeof(cache_13));
	nueva_entrada->direccion_posta = direccion_posta;
	nueva_entrada->esta_en_uso = true;
	nueva_entrada->nro_pagina = nro_pagina;
	nueva_entrada->pid = pid;
	list_add(*tlb, nueva_entrada);
	return 'f'; // f = fifo
}

void borrame_las_entradas_del_proceso(int pid, t_list ** tlb) {
	int i,j,k;
	t_list * entradas_a_borrar = list_create();
	int size = list_size(*tlb);
	for(i = 0; i < size ; i++) {
		cache_13 * aux = list_get(*tlb, i);
		if(aux->pid == pid && aux->esta_en_uso) {
			int * aux = malloc(sizeof(int));
			*aux = i;
			list_add(entradas_a_borrar, aux);
		}
	}
	int cant_entradas_a_borrar = list_size(entradas_a_borrar);
	for(j = 0; j < cant_entradas_a_borrar ; j++) {
		int * list = list_remove(entradas_a_borrar, list_size(entradas_a_borrar) - 1);
		free(list_remove(*tlb, *list));
		free(list);
	}
	for(k = 0 ; k < cant_entradas_a_borrar ; k++) {
		cache_13 * nueva_entrada = malloc(sizeof(cache_13));
		nueva_entrada->esta_en_uso = false;
		nueva_entrada->pid = -1;
		list_add(*tlb, nueva_entrada);
	}
}

void limpiar_la_tlb(t_list ** tlb){
	int cant_entradas = list_size(*tlb);
	while(!list_is_empty(*tlb))
		free(list_remove(*tlb,0));
	*tlb = inicializar_tlb(cant_entradas);
}

void limpiar_memoria(t_list ** tabla_de_paginas, char * memoria, int tamanio_marco, int socket_swap) {
	int i;
	for(i = 0 ; i< list_size(*tabla_de_paginas) ; i++) {
		tabla_paginas * tabla = list_get(*tabla_de_paginas, i);
		int j;
		for (j = 0 ; j < list_size(tabla->list_pagina_direccion) ; j++) {
			pagina_direccion * pagina = list_get(tabla->list_pagina_direccion, j);
			if (pagina->en_uso && pagina->fue_modificado){
				char * mensaje = dame_mensaje_de_memoria(&memoria, pagina->nro_marco, tamanio_marco);
				tprotocolo_desde_cpu_y_hacia_swap paquete_a_swap;
				armar_estructura_desde_cpu_y_hacia_swap(&paquete_a_swap, 'e', tabla->pid, pagina->nro_pagina, mensaje);
				void* buffer = serializar_a_swap(&paquete_a_swap);
				send(socket_swap, buffer, strlen(mensaje) + 13, 0);
				free(buffer);
				free(mensaje);
			}
			pagina->en_uso = false;
		}
	}
}

void volcar_memoria(char * memoria, tconfig_memoria * config, t_log * logMem) {
	int buffer_memoria = 0;
	while (buffer_memoria < config->tamanio_marco * config->cantidad_marcos) {
		char * mensaje = malloc(config->tamanio_marco + 1);
		memcpy(mensaje, memoria + buffer_memoria, config->tamanio_marco);
		mensaje[config->tamanio_marco] = '\0';
		log_info(logMem, "Numero de marco %d: %s", buffer_memoria / config->tamanio_marco, mensaje);
		buffer_memoria += config->tamanio_marco;
	}
}

int dame_el_numero_de_entrada_de_la_tlb(t_list * tlb, int direccion_posta) {
	int i;
	for(i = 0; i < list_size(tlb) ; i++) {
		cache_13 * aux = list_get(tlb, i);
		if(direccion_posta == aux->direccion_posta)
			return i;
	}
	return -1;
}
