#include <stdlib.h>
#include "estructuras.h"
#include <commons/collections/list.h>
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
	for(i = 0 ; i < list_size(*lista_tabla_de_paginas) ; i++) {
		tabla_paginas *tabla = list_get(*lista_tabla_de_paginas, i);
		if(tabla->pid == pid)
			free(list_remove(*lista_tabla_de_paginas, i));
	}
}

tabla_paginas *dame_la_tabla_de_paginas(int pid, t_list ** lista_tabla_de_paginas) {
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
		pagina_direccion *tabla_aux = list_get(tabla->list_pagina_direccion, i);

		if(tabla_aux->en_uso && tabla_aux->nro_pagina == pagina)
			return tabla_aux->nro_marco;
	}
	return -1;
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
	for(i = 0; i < cantidad_marcos ; i++) {

		bool libre = true;

		for(j = 0; j < list_size(lista_tabla_de_paginas); j++) {

			bool no_libre = false;

			tabla_paginas *tabla = list_get(lista_tabla_de_paginas, j);
			for(k = 0; k < list_size(tabla->list_pagina_direccion); k++) {

				pagina_direccion *pagina = list_get(tabla->list_pagina_direccion, k);
				if(pagina->nro_marco == i && pagina->en_uso) {
					no_libre = true;
					libre = false;
					break;
				}
			}
			if (no_libre)
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

void actualizame_la_tlb(t_list ** tlb, int pid, int direccion_posta, int nro_pagina) {
	int i;
	for(i = 0; i< list_size(*tlb); i++) {
		cache_13 * aux = list_get(*tlb, i);
		if(!aux->esta_en_uso) {
			aux->direccion_posta = direccion_posta;
			aux->esta_en_uso = true;
			aux->pid = pid;
			aux->nro_pagina = nro_pagina;
			return;
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

}

void borrame_las_entradas_del_proceso(int pid, t_list ** tlb) {
	int i;
	for(i = 0; i< list_size(*tlb); i++) {
		cache_13 * aux = list_get(*tlb, i);

		if(aux->pid == pid) {
			free(list_remove(*tlb, i));

			cache_13 * nueva_entrada = malloc(sizeof(cache_13));
			nueva_entrada->esta_en_uso = false;
			list_add(*tlb, nueva_entrada);
		}
	}
}
