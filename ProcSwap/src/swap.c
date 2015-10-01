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
	system("clear");
 	tconfig_swap* config_swap = leerConfiguracion();

	// inicializamos lista de ocupados
	t_list * lista_ocupado = list_create();
	t_list * lista_vacia = list_create();

	FILE *swap = iniciar_archivo_swap();


	//tlista_vacio *vacio = malloc(sizeof(tlista_vacio));
	//vacio->comienzo = 0;
	//vacio->paginas_vacias = config_swap->cantidadPaginas;
	//list_add(lista_vacia, vacio);


	//t_log *logSwap = log_create("../src/log.txt", "swap.c", false, LOG_LEVEL_INFO);

	//Crea el socket para escuchar
	int serverSocket;
	server_init(&serverSocket, "4142");
	printf("SWAP listo...\n");

	//log_info(logSwap, "SWAP iniciado");

	//Crea el socket para recibir a la memoria
	int socketMemoria;
	server_acept(serverSocket, &socketMemoria);
	printf("Memoria aceptada...\n");

	//log_info(logSwap, "Conectado a la memoria");



/// /////////////////////////////////////////////////////////////////////



	//comienzo de pasaje de datos


	tlista_vacio *lv = malloc(sizeof(tlista_vacio));

	lv->comienzo = 3;
	lv->paginas_vacias = 3;
	list_add(lista_vacia, lv);
	tlista_vacio *l2 = malloc(sizeof(tlista_vacio));

	l2->comienzo = 8;
	l2->paginas_vacias = 1;
	list_add(lista_vacia, l2);

	tlista_ocupado *nodo1 = malloc(sizeof(tlista_ocupado));
			nodo1->pid = 1;
			nodo1->comienzo = 0;
			nodo1->paginas_ocupadas = 2;
			tlista_ocupado *nodo2 = malloc(sizeof(tlista_ocupado));
			nodo2->pid = 2;
			nodo2->comienzo = 2;
			nodo2->paginas_ocupadas = 1;
			tlista_ocupado *nodo3 = malloc(sizeof(tlista_ocupado));
			nodo3->pid = 3;
			nodo3->comienzo = 6;
			nodo3->paginas_ocupadas = 2;

			list_add(lista_ocupado, nodo1);
			list_add(lista_ocupado, nodo2);
			list_add(lista_ocupado, nodo3);


//////////////
	tprotocolo_memoria_swap protocolo_desde_memoria;
	recibir_paquete_desde_memoria(&socketMemoria, &protocolo_desde_memoria);


	// muestro como quedan las listas
/*
	tlista_vacio *v = malloc(sizeof(tlista_vacio));
	v = list_get(lista_vacia,0);
		printf("comienzo:  %d\n", v->comienzo );
		printf("cantidad paginas: %d\n", v->paginas_vacias);
*/




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



		switch(protocolo_desde_memoria.codigo_op){

			//inicializar probado
			case 'i': {
				int comienzo, hay_espacio;
				hay_espacio = dame_si_hay_espacio(&lista_vacia, protocolo_desde_memoria.cantidad_pagina, &comienzo);

				if (hay_espacio) {
					//asignar el espacio solicitado
					tlista_ocupado *ocupado = malloc(sizeof(tlista_ocupado));
					ocupado->pid = protocolo_desde_memoria.pid;
					ocupado->comienzo = comienzo;
					ocupado->paginas_ocupadas = protocolo_desde_memoria.cantidad_pagina;
					list_add(lista_ocupado, ocupado);
				}
				else {
					if (espacio_total_disponible(lista_vacia) >= protocolo_desde_memoria.cantidad_pagina){
						//compactamos, y retorno el comienzo del espacio vacio
						int comienzo = compactar_swap(&swap, &lista_vacia, &lista_ocupado, config_swap->tamanioPagina, config_swap->cantidadPaginas);

						//ocupo espacio
						tlista_ocupado *ocupado = malloc(sizeof(tlista_ocupado));
						ocupado->pid = protocolo_desde_memoria.pid;
						ocupado->comienzo = comienzo;
						ocupado->paginas_ocupadas = protocolo_desde_memoria.cantidad_pagina;
						list_add(lista_ocupado, ocupado);
						// actualizar la lista de vacios, con los espacios vacios que resultaron de compactar menos los solicitados

						tlista_vacio *aux = list_get(lista_vacia, 0);
						tlista_vacio *update = malloc(sizeof(tlista_vacio));
						*update = *aux;

						update->comienzo += protocolo_desde_memoria.cantidad_pagina;
						update->paginas_vacias -= protocolo_desde_memoria.cantidad_pagina;
						list_destroy_and_destroy_elements(lista_vacia,free);

						lista_vacia = list_create();
						list_add(lista_vacia, update);

					}
					else {
						printf("avisamos ");
					}
				}
			}
			break;

			//finalizar probado
			case 'f':

			{
				int i;
				for (i = 0; i < list_size(lista_ocupado); i++){

					tlista_ocupado * espacio_ocupado = list_get(lista_ocupado, i);

					if (espacio_ocupado->pid == protocolo_desde_memoria.pid) {

						//agrego a la lista vacia el espacio que voy a liberar
						tlista_vacio * espacio_vacio = malloc(sizeof(tlista_vacio));
						espacio_vacio->comienzo = espacio_ocupado->comienzo;
						espacio_vacio->paginas_vacias = espacio_ocupado->paginas_ocupadas;
						list_add(lista_vacia,espacio_vacio);

						//saco espacio de lista ocupado
						list_remove(lista_ocupado, i);

					}
				}
			}

			break;


			//leer pagina
			case 'l':

			{
				int pag_inicio = get_comienzo_espacio_asignado(lista_ocupado, protocolo_desde_memoria.pid);
				printf("pag_inicio : %d\n", pag_inicio);
				//indica la pagina a leer
				int pag_leer = protocolo_desde_memoria.cantidad_pagina;
				printf("pag_leer : %d\n", pag_leer);
				//me posiciono sobre la pagina a leer
				int desplazamiento_en_bytes = (pag_inicio + pag_leer)*config_swap->tamanioPagina;
				printf("desplazamiento_en_bytes : %d\n", desplazamiento_en_bytes);
				fseek(swap, desplazamiento_en_bytes, SEEK_SET);

				char * pag_data = malloc(config_swap->tamanioPagina + 1);
				fread(pag_data, config_swap->tamanioPagina, 1, swap);
				//devolverle lo leido a la memoria --> cpu

			}

			break;

			//escribir pagina
			case 'e':
				break;
		}



		free(protocolo_desde_memoria.mensaje);







	//close(serverSocket);
	//log_info(logSwap, "SWAP finalizado");

	//close(socketMemoria);
	//log_info(logSwap, "Cerrada conexion con memoria");

	fclose(swap);
	//free(config_swap);

	return 0;
}
