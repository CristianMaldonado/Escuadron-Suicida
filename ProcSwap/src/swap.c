#include <stdio.h>
#include <stdlib.h>
#include "../../lib/libSocket.h"
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include "estructuras.h"
#include "manejo_listas_archivo_swap.h"
#include "paquetes.h"
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "config.h"
#include <string.h>




int main(void) {

 	tconfig_swap* config_swap = leerConfiguracion();

	// inicializamos lista de ocupados
	t_list * lista_ocupado = list_create();
	t_list * lista_vacia = list_create();

	FILE *swap = iniciar_archivo_swap();

	//iniciamos la lista de paginas vacias
	tlista_vacio *vacio = malloc(sizeof(tlista_vacio));
	vacio->comienzo = 0;
	vacio->paginas_vacias = config_swap->cantidadPaginas;
	list_add(lista_vacia, vacio);

	//creacion de la instancia de log
	//t_log *logSwap = log_create("../src/log.txt", "swap.c", false, LOG_LEVEL_INFO);


/*

	//Crea el socket para escuchar
	int serverSocket;
	server_init(&serverSocket, "4141");
	printf("SWAP listo...\n");

	// loguea Swap iniciado
	//log_info(logSwap, "SWAP iniciado");

	//Crea el socket para recibir a la memoria
	int socketMemoria;
	server_acept(serverSocket, &socketMemoria);
	printf("Memoria aceptada...\n");

	// loguea conexion con Memoria
	//log_info(logSwap, "Conectado a la memoria");

*/





/// /////////////////////////////////////////////////////////////////////



	//comienzo de pasaje de datos


	/*

	tprotocolo_memoria_swap prot;
	int salir = 0;
	while(!salir){

		if(recibir_paquete_desde_memoria(&socketMemoria, &prot))
			// para probar si recibe
			printf("%s\n", prot.mensaje);
		else
			// si no recibe termina el swap
			salir = 1;
*/

	// recibo desde memoria una sola vez, para probar

	tprotocolo_memoria_swap protocolo;



		if(protocolo.codigo_op == 'i') {
			int comienzo = dame_si_hay_espacio(lista_vacia, protocolo.cantidad_pagina);

			printf("%d", comienzo);


			if (comienzo >= 0){
				//ocupo espacio
				tlista_ocupado *ocupado = malloc(sizeof(tlista_ocupado));
				ocupado->pid = protocolo.pid;
				ocupado->comienzo = comienzo;
				ocupado->paginas_ocupadas = protocolo.cantidad_pagina;
				list_add(lista_ocupado, (void*)ocupado);
			}
			else {
				if (espacio_total_disponible(lista_vacia) >= protocolo.cantidad_pagina){
					//compactamos
					int comienzo = compactar_swap(&swap, &lista_vacia, &lista_ocupado, config_swap->tamanioPagina, config_swap->cantidadPaginas);
					//ocupo espacio
					tlista_ocupado *ocupado = malloc(sizeof(tlista_ocupado));
					ocupado->pid = protocolo.pid;
					ocupado->comienzo = comienzo;
					ocupado->paginas_ocupadas = protocolo.cantidad_pagina;
					list_add(lista_ocupado, (void*)ocupado);
				}
				else {
					printf("avisamos ");
				}
			}
		}

/*
		switch(prot.codigo_op){

			//inicializar
			case 'i':
			{
				int comienzo = dame_si_hay_espacio(lista_vacia, prot.cantidad_pagina);
				if (comienzo >= 0){

					//ocupo espacio
					tlista_ocupado *ocupado = malloc(sizeof(tlista_ocupado));
					ocupado->pid = prot.pid;
					ocupado->comienzo = comienzo;
					ocupado->paginas_ocupadas = prot.cantidad_pagina;
					list_add(lista_ocupado, (void*)ocupado);


				}


				else
				{
					if (espacio_total_disponible(lista_vacia) >= prot.cantidad_pagina){

						//compactamos
						int comienzo = compactar_swap(&swap, &lista_vacia, &lista_ocupado, config_swap->tamanioPagina, config_swap->cantidadPaginas);

						//ocupo espacio
						tlista_ocupado *ocupado = malloc(sizeof(tlista_ocupado));
						ocupado->pid = prot.pid;
						ocupado->comienzo = comienzo;
						ocupado->paginas_ocupadas = prot.cantidad_pagina;
						list_add(lista_ocupado, (void*)ocupado);

					}
					else
					{
						//le avisamos que explote
					}
				}
			}


				break;

			//finalizar
			case 'f':

			{
				int i;
				for (i = 0; i < list_size(lista_ocupado); i++){

					tlista_ocupado * espacio_ocupado;
					espacio_ocupado = list_get(lista_ocupado, i);

					if (espacio_ocupado->pid == prot.pid){

						//saco espacio de lista ocupado
						list_remove(lista_ocupado, i);

						//agrego a la lista vacia el espacio que voy a liberar
						tlista_vacio * espacio_vacio = malloc(sizeof(tlista_vacio));
						espacio_vacio->comienzo = espacio_ocupado->comienzo;
						espacio_vacio->paginas_ocupadas = espacio_ocupado->paginas_ocupadas;
						list_add(lista_vacia,espacio_vacio);
					}
				}
			}

			break;


			//leer pagina
			case 'l':

			{
				int pag_inicio = get_comienzo_espacio_asignado(lista_ocupado, prot.pid);
				//indica la pagina a leer
				int pag_leer = prot.cantidad_pagina;

				//me posiciono sobre la pagina a leer
				int desplazamiento_en_bytes = (pag_inicio + pag_leer)*config_swap->tamanioPagina;
				fseek(swap, SEEK_SET, desplazamiento_en_bytes);

				char * pag_data = malloc(config_swap->tamanioPagina + 1);
				fread(pag_data, config_swap->tamanioPagina, 1, swap);

				//algo

			}

			break;

			//escribir pagina
			case 'e':
				break;
		}



		free(prot.mensaje);
	}

*/






	//close(serverSocket);
	//log_info(logSwap, "SWAP finalizado");

	//close(socketMemoria);
	//log_info(logSwap, "Cerrada conexion con memoria");

	fclose(swap);
	//free(config_swap);

	return 0;
}
