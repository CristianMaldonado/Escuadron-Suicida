/*
 * estructuras_swap.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef SRC_MANEJO_LISTAS_ARCHIVO_SWAP_H_
#define SRC_MANEJO_LISTAS_ARCHIVO_SWAP_H_

int get_comienzo_espacio_asignado(t_list * lista_ocupado, int pid);
int dame_si_hay_espacio(t_list* lista_vacia, int paginas_pedidas);
int espacio_total_disponible(t_list* lista_vacia);
t_list *pasar_ocupada_a_lista_auxiliar(FILE **swap, t_list **lista_ocupada, int tamanio_pagina);
void reinicar_archivo_swap(FILE **swap, t_list **lista_ocupada);
void lista_vacia_compactada(t_list **lista_vacia, FILE **swap, int tamanio_pagina ,int total_de_paginas);


#endif /* SRC_MANEJO_LISTAS_ARCHIVO_SWAP_H_ */
