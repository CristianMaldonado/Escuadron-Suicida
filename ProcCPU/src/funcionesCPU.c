/*
 * funcionesCPU.c
 *
 *  Created on: 23/9/2015
 *      Author: utnso
 */

#include "funcionesCPU.h"
#include <commons/error.h>

int interpretarInstruccion(char* instruccion){

	if(string_starts_with(instruccion,"iniciar")){ return 1; }
	if(string_starts_with(instruccion,"leer")) { return 2; }
	if(string_starts_with(instruccion,"escribir")) { return 3; }
	if(string_starts_with(instruccion,"entrada-salida")) { return 4; }
	if(string_starts_with(instruccion,"finalizar")) { return 5; }
	else{ return 0; }
}

