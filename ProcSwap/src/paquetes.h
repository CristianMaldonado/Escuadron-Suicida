/*
 * paquetes.h
 *
 *  Created on: 25/9/2015
 *      Author: utnso
 */

#ifndef SRC_PAQUETES_H_
#define SRC_PAQUETES_H_

FILE * iniciar_archivo_swap(void);

void recibir_paquete_desde_memoria(int socket_memoria, tprotocolo_memoria_swap *paquete_desde_memoria);
void* serializar_a_memoria(tprotocolo_swap_memoria *protocolo);


#endif /* SRC_PAQUETES_H_ */
