#include <sys/types.h>
#include <sys/socket.h>
#include "libSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include "estructuras.h"
#include "manejo_listas_archivo_swap.h"
#include <stdbool.h>
#include "config.h"
#include <string.h>
#include "paquetes.h"
#include <unistd.h>



int main(void) {
	system("clear");


	//t_log *logSwap = log_create("log.txt", "swap.c", false, LOG_LEVEL_INFO);

 	//tconfig_swap* config_swap = leerConfiguracion();

	// inicializamos lista de ocupados
	t_list * lista_ocupado = list_create();
	t_list * lista_vacia = list_create();

	//FILE *swap = iniciar_archivo_swap();


/*	tlista_vacio *vacio = malloc(sizeof(tlista_vacio));
	vacio->comienzo = 0;
	vacio->paginas_vacias = config_swap->cantidadPaginas;
	list_add(lista_vacia, vacio);
*/

/*	//Crea el socket para escuchar
	int server_socket;
	server_init(&server_socket, "4142");
	printf("SWAP listo...\n");

	//Crea el socket para recibir a la memoria
	int socket_memoria;
	server_acept(server_socket, &socket_memoria);
	printf("Memoria aceptada...\n");
*/




	//Se recibe desde memoria
	//tprotocolo_memoria_swap protocolo_desde_memoria;
//	recibir_paquete_desde_memoria(&socket_memoria, &protocolo_desde_memoria);

/*
	printf("\n imprimir paquete desde memoria: \n");
	printf("\ncantidad_pag: %d\n", protocolo_desde_memoria.cantidad_pagina);
	printf("\npid: %d\n", protocolo_desde_memoria.pid);
	printf("\ncod_op: %c\n", protocolo_desde_memoria.codigo_op);
	printf("\ntamanio: %d\n", protocolo_desde_memoria.tamanio_mensaje);
	printf("\nmensaje: %s\n", protocolo_desde_memoria.mensaje);

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
/*
		switch(protocolo_desde_memoria.codigo_op){

			//inicializar probado
			case 'i': {
				char codaux = 'i';
				int comienzo, hay_espacio;
				hay_espacio = dame_si_hay_espacio(&lista_vacia, protocolo_desde_memoria.cantidad_pagina, &comienzo);
				if (hay_espacio) {
					//asignar el espacio solicitado
					tlista_ocupado *ocupado = malloc(sizeof(tlista_ocupado));
					ocupado->pid = protocolo_desde_memoria.pid;
					ocupado->comienzo = comienzo;
					ocupado->paginas_ocupadas = protocolo_desde_memoria.cantidad_pagina;
					list_add(lista_ocupado, ocupado);

					log_inicializar(logSwap,protocolo_desde_memoria.pid,ocupado->comienzo,config_swap->tamanioPagina,protocolo_desde_memoria.cantidad_pagina);
				}
				else {
					if (espacio_total_disponible(lista_vacia) >= protocolo_desde_memoria.cantidad_pagina){
						//compactamos, y retorno el comienzo del espacio vacio
						log_info(logSwap,"compactacion iniciada /n");
						int comienzo = compactar_swap(&swap, &lista_vacia, &lista_ocupado, config_swap->tamanioPagina, config_swap->cantidadPaginas);
						log_info(logSwap,"compactacion finalizada /n");

						//ocupo espacio
						tlista_ocupado *ocupado = malloc(sizeof(tlista_ocupado));
						ocupado->pid = protocolo_desde_memoria.pid;
						ocupado->comienzo = comienzo;
						ocupado->paginas_ocupadas = protocolo_desde_memoria.cantidad_pagina;
						list_add(lista_ocupado, ocupado);

						log_inicializar(logSwap,protocolo_desde_memoria.pid,ocupado->comienzo,config_swap->tamanioPagina,protocolo_desde_memoria.cantidad_pagina);

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
					else
						codaux = 'a';
						log_proc_rechazado(logSwap, protocolo_desde_memoria.pid);
				}
				tprotocolo_swap_memoria swap_memoria;
								armar_estructura_protocolo_a_memoria(&swap_memoria,codaux, protocolo_desde_memoria.pid, "-");
								void * buffer = serializar_a_memoria(&swap_memoria);
	//							send(socket_memoria, buffer, 9 + strlen("-"), 0);
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

						log_finalizar(logSwap,espacio_ocupado->pid,config_swap->tamanioPagina, espacio_ocupado->paginas_ocupadas);
					}
				}
			}

			break;


			//leer pagina
			case 'l': {
				int pag_inicio = get_comienzo_espacio_asignado(lista_ocupado, protocolo_desde_memoria.pid);

				//indica la pagina a leer
				int pag_leer = protocolo_desde_memoria.cantidad_pagina;

				//me posiciono sobre la pagina a leer
				int desplazamiento_en_bytes = (pag_inicio + pag_leer)*config_swap->tamanioPagina;
				fseek(swap, desplazamiento_en_bytes, SEEK_SET);

				char * pag_data = malloc(config_swap->tamanioPagina);
				fread(pag_data, config_swap->tamanioPagina, 1, swap);
				pag_data[config_swap->tamanioPagina] = '\0';

				log_escritura(logSwap, protocolo_desde_memoria.pid, pag_inicio,config_swap->tamanioPagina, pag_leer, pag_data);

				tprotocolo_swap_memoria swap_memoria;
				armar_estructura_protocolo_a_memoria(&swap_memoria,'i', protocolo_desde_memoria.pid, pag_data);
				void * buffer = serializar_a_memoria(&swap_memoria);
		//		send(socket_memoria, buffer, 9 + config_swap->tamanioPagina, 0);
			}

			break;

			//escribir pagina
			case 'e':
				break;
		}
		free(protocolo_desde_memoria.mensaje);

*/


	FILE* swap = fopen("swap.dat", "rb+");
//	fseek(swap, 0, SEEK_SET);

		// armo lista test vacio
			tlista_vacio *vacio1 = malloc(sizeof(tlista_vacio));
			tlista_vacio *vacio2 = malloc(sizeof(tlista_vacio));

			vacio1->comienzo = 3;
			vacio1->paginas_vacias = 3;
			vacio2->comienzo = 8;
			vacio2->paginas_vacias = 1;

			list_add(lista_vacia, vacio1);
			list_add(lista_vacia, vacio2);
			//int aux = espacio_total_disponible(lista_vacia);
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
				nodo3->comienzo = 6;
				nodo3->paginas_ocupadas = 2;

				list_add(lista_ocupado, nodo1);
				list_add(lista_ocupado, nodo2);
				list_add(lista_ocupado, nodo3);

				// compacto
				printf("nuevo comienzo\n %d: ",compactar_swap(&swap, &lista_vacia, &lista_ocupado, 4, 9));

		//		fseek(swap, 20, SEEK_SET);
			//	lista_vacia_compactada(&lista_vacia, &swap, 4 ,9);

				//list_destroy_and_destroy_elements(lista_vacia, free);


				/*
				tlista_vacio *v = malloc(sizeof(tlista_vacio));
				//list_add(lista_vacia, vacio1);

				v = list_get(lista_vacia, 0);

				printf("vacias: %d\n", v->paginas_vacias);
				printf("comienzo: %d\n", v->comienzo);

				free(v);
*/

				/*tlista_ocupado *o = malloc(sizeof(tlista_ocupado));
				o = list_get(lista_ocupado, 2);

				printf("\n%d\n", o->pid);

				printf("\n%d\n", o->comienzo);
				printf("\n%d\n", o->paginas_ocupadas);

*/

				char m[36];
				fseek(swap, 0 ,SEEK_SET);
				fread(m, sizeof(char), 36, swap);

				printf("%s\n\n", m);


























	//close(socket_memoria);
	//close(server_socket);
	fclose(swap);
	//free(config_swap);
	return 0;
}
