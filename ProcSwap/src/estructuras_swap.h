/*
 * estructuras_swap.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef SRC_ESTRUCTURAS_SWAP_H_
#define SRC_ESTRUCTURAS_SWAP_H_

int get_comienzo_espacio_asignado(t_list * lista_ocupado, int pid);
int dame_si_hay_espacio(t_list* lista_vacia, int paginas_pedidas);
int espacio_total_disponible(t_list* lista_vacia);




#endif /* SRC_ESTRUCTURAS_SWAP_H_ */
