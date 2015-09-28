#include <pthread.h>
#include "../../lib/libSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "estructuras.h"
#include "paquetes.h"
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "config.h"



//devuelve la posicion del espacio libre disponible
int compactar_swap(FILE ** swap, t_list** lista_vacia, t_list** lista_ocupada,int tamanio_pagina, int total_de_paginas){
	t_list *lista_aux = list_create();
	//pasamos los datos a una lista
	int count = 0;
	while (!list_is_empty(*lista_ocupada)){
		tlista_ocupado *elem = malloc(sizeof(tlista_ocupado));
		elem = list_remove(*lista_ocupada, count);
		tdatos_paginas *data = malloc(sizeof(tdatos_paginas));
		data->pid = elem->pid;
		data->tamanio = elem->paginas_ocupadas*tamanio_pagina;//en bytes
		data->buffer = (char*)malloc(data->tamanio);// puede haber igual o menor del tamanio
		//leemos los datos
		fseek(*swap,SEEK_SET,elem->comienzo*tamanio_pagina);
		fread(data->buffer, sizeof(char), tamanio_pagina*elem->paginas_ocupadas , *swap); // lee y guarda en buffer pero tiene ceros al final
		list_add(lista_aux,data);
		count++;
	}
	//reiniciar swap
	fclose(*swap);
	*swap = iniciar_archivo_swap();
	list_destroy_and_destroy_elements(*lista_ocupada, free);
	//compactamos la swap
	int cont_pagina = 0;
	int nodo_lista_aux = 0;
	while (!list_is_empty(lista_aux)){
		tdatos_paginas *elem = malloc(sizeof(tdatos_paginas));
		elem = list_remove(lista_aux, nodo_lista_aux);
		fwrite(elem->buffer, elem->tamanio, 1, *swap);
		//actualizamos lista ocupada
		tlista_ocupado *elem_ocupada = malloc(sizeof (tlista_ocupado));
		elem_ocupada->pid = elem->pid;
		elem_ocupada->comienzo = cont_pagina;
		elem_ocupada->paginas_ocupadas = (int)elem->tamanio/tamanio_pagina;
		cont_pagina += elem_ocupada->paginas_ocupadas;
		list_add(*lista_ocupada, elem_ocupada);
		nodo_lista_aux++;
	}
	list_destroy_and_destroy_elements(lista_aux, free);
	//actualizar lista vacia
	list_destroy_and_destroy_elements(*lista_vacia, free);
	tlista_vacio *vacio = malloc(sizeof(lista_vacia));
	vacio->comienzo = ftell(*swap)/tamanio_pagina;
	vacio->paginas_vacias = total_de_paginas - vacio->comienzo;
	list_add(*lista_vacia, vacio);
	return vacio->comienzo;
}

int main(void) {


 	//tconfig_swap* config_swap = leerConfiguracion();

	// inicializamos lista de ocupados
	t_list * lista_ocupado = list_create();
	t_list * lista_vacia = list_create();

	//tlista_vacio *vacio = malloc(sizeof(tlista_vacio));

	//iniciamos en cero el archivo swap
	//FILE *swap = iniciar_archivo_swap();

	FILE *swap = fopen("swap.txt", "r");

	char l[36];
	fread(l, sizeof(char), 36, swap);
	printf("%s\n\n", l);

	printf("\n\nespacio\n\n");


	//iniciamos la lista de paginas vacias
	//vacio->comienzo = 0;
	//vacio->paginas_vacias = config_swap->cantidadPaginas;
	//list_add(lista_vacia, vacio);


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

	//////////////////////////////////////////////////////////////////////pruebas///////////////////

	/* inicializamos lista de ocupados
		t_list * lista_ocupado = list_create();
		t_list * lista_vacia = list_create();

		tlista_vacio *vacio = malloc(sizeof(tlista_vacio));
*/


	// armo lista test vacio
	tlista_vacio *vacio1 = malloc(sizeof(tlista_vacio));
	tlista_vacio *vacio2 = malloc(sizeof(tlista_vacio));

	vacio1->comienzo = 3;
	vacio1->paginas_vacias = 3;
	vacio2->comienzo = 9;
	vacio2->paginas_vacias = 1;

	list_add(lista_vacia, vacio1);
	list_add(lista_vacia, vacio2);

	int aux = espacio_total_disponible(lista_vacia);
	//printf("total disponible de paginas: %d\n", aux);


	// armo lista test ocupado
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
		nodo3->comienzo = 7;
		nodo3->paginas_ocupadas = 2;

		list_add(lista_ocupado, nodo1);
		list_add(lista_ocupado, nodo2);
		list_add(lista_ocupado, nodo3);

		//printf("%d\n", list_size(lista_ocupado));
		tlista_ocupado *oc = malloc(sizeof(tlista_ocupado));
		oc = list_get(lista_ocupado, 0);
		//printf("%d\n", oc->comienzo);

		int resultado_de_get = get_comienzo_espacio_asignado(lista_ocupado, 4);
		//printf("comienzo con el pid 2 : %d\n", resultado_de_get);



		// compacto
		compactar_swap(&swap, &lista_vacia, &lista_ocupado, 4, 9);






		char m[40];
		fseek(swap, 0l, SEEK_SET);
		fread(m, sizeof(char), 40, swap);
			printf("\n%s\n", l);
















/// /////////////////////////////////////////////////////////////////////77



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


		if(prot.codigo_op == 'i') {
			int comienzo = dame_si_hay_espacio(lista_vacia, prot.cantidad_pagina);

			printf("%d", comienzo);


			/*if (comienzo >= 0){
				//ocupo espacio
				tlista_ocupado *ocupado = malloc(sizeof(tlista_ocupado));
				ocupado->pid = prot.pid;
				ocupado->comienzo = comienzo;
				ocupado->paginas_ocupadas = prot.cantidad_pagina;
				list_add(lista_ocupado, (void*)ocupado);
			}


			else {
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
				else {
					printf("avisamos ");
				}

			}
		}



		}
*/


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

		*/

//		free(prot.mensaje);
//	}








	//close(serverSocket);
	//log_info(logSwap, "SWAP finalizado");

	//close(socketMemoria);
	//log_info(logSwap, "Cerrada conexion con memoria");

	fclose(swap);
	//free(config_swap);

	return 0;
}
