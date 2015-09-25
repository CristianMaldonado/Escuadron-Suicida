/*
 * paquetes.h
 *
 *  Created on: 25/9/2015
 *      Author: utnso
 */

#ifndef SRC_PAQUETES_H_
#define SRC_PAQUETES_H_


void des_serializar_cpu(void* buffer, tprotocolo *paquete_Desde_Cpu);
void armar_estructura_protocolo(tprotocolo *protocolo, char cod_op, int pid, int paginas, char* mensaje);
void* serializar_a_swap(tprotocolo *protocolo);



#endif /* SRC_PAQUETES_H_ */
