/*
 * log_memoria.h
 *
 *  Created on: 31/10/2015
 *      Author: utnso
 */

#ifndef SRC_LOG_MEMORIA_H_
#define SRC_LOG_MEMORIA_H_

#include <stdbool.h>

void log_inicializar(t_log *log, int pid, int paginas_asignadas);
void log_lectura_escritura(char operacion, char * resultado, t_log *log, int pid, int nro_pagina, int numero_entrada_en_tlb, bool acierto_tlb, int nro_marco_resultante);
void log_acceso_memoria(t_log * log, int pid, int pagina, int nro_marco);
void log_acceso_a_swap(t_log * log, int pid, int pagina);
void log_seniales(t_log * log, char * mensaje);

#endif /* SRC_LOG_MEMORIA_H_ */
