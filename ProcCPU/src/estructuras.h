/*
 * estructuras.h
 *
 *  Created on: 11/9/2015
 *      Author: utnso
 */

#ifndef SRC_ESTRUCTURAS_H_
#define SRC_ESTRUCTURAS_H_

typedef struct {
	char* ipPlanificador;
	char* puertoPlanificador;
	char* ipMemoria;
	char* puertoMemoria;
	int cantidadHilos;
	int retardo;
} tipoConfiguracionCPU;

typedef struct {
	int pid;
	char* path;
	char* nombrePrograma;
	char* estadoProceso;
	int punteroInstruccion;
} t_pcb;

typedef struct {
	char tipoMensaje;
	int tamanioMensaje;
	char* mensaje;
} protocolo_planificador_cpu;                            //TO DO: REVISARRR

typedef struct {
	char codOperacion;
	char codAux;
	int pid;
	int nroPagina;
	char* mensaje;
} protocolo_cpu_memoria;

typedef struct{
	int socket;
	char* lineaDeProceso;
	protocolo_cpu_memoria paquete;
}tMensajeAMemoria;

#endif /* SRC_ESTRUCTURAS_H_ */
