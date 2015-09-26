/*
 * paquetes.h
 *
 *  Created on: 25/9/2015
 *      Author: utnso
 */

#ifndef SRC_PAQUETES_H_
#define SRC_PAQUETES_H_


void armar_estructura_protocolo_mem_y_swap(tprotocolo_desde_cpu_y_hacia_swap *protocolo, char cod_op, int pid, int paginas, char* mensaje);
void* serializar_a_swap(tprotocolo_desde_cpu_y_hacia_swap *protocolo);
void recibir_paquete_desde_cpu(int *socket_cpu, tprotocolo_desde_cpu_y_hacia_swap *paquete_desde_cpu);
void recibir_paquete_desde_swap(int socket_swap, tprotocolo_swap_memoria *paquete_desde_swap);
void armar_estructura_protocolo_a_cpu(tprotocolo_memoria_cpu *protocolo, char cod_op, char cod_aux, int pid, int numero_pagina, char* mensaje);
void* serializar_a_cpu(tprotocolo_memoria_cpu *protocolo);


#endif /* SRC_PAQUETES_H_ */