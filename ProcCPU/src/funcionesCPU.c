/*
 * funcionesCPU.c
 *
 *  Created on: 23/9/2015
 *      Author: utnso
 */

#include "funcionesCPU.h"

void interpretarInstruccion(char* instruccion){

	if(string_starts_with(instruccion,"iniciar")){ pedirMemoria(); }
	if(string_starts_with(instruccion,"leer")) { leerPagina(); }
	if(string_starts_with(instruccion,"escribir")) { escribirTexto(); }
	if(string_starts_with(instruccion,"entrada-salida")) { tenesQueEsperar(); }
	if(string_starts_with(instruccion,"finalizar")) { processKill(); }
}

void pedirMemoria(){

}

void leerPagina(){

}

void escribirTexto(){

}

void tenesQueEsperar(){

}

void processKill(){

}
