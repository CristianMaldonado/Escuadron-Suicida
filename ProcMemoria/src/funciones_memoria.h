#ifndef SRC_FUNCIONES_MEMORIA_H_
#define SRC_FUNCIONES_MEMORIA_H_

#include "estructuras.h"
#include <commons/collections/list.h>

char * crear_memoria(int cantidad_marcos, int tamanio_marcos);
tabla_paginas * inicializar_tabla_de_paginas(int cantidad_maxima_marcos_por_proceso, int pid);
void eliminar_tabla_de_proceso(int pid, t_list ** lista_tabla_de_paginas);
tabla_paginas * dame_la_tabla_de_paginas(int pid, t_list ** lista_tabla_de_paginas);
int dame_la_direccion_de_la_pagina(tabla_paginas *tabla, int pagina);
bool estan_los_frames_ocupados(t_list *tabla_paginas);
int dame_un_marco_libre(t_list *lista_tabla_de_paginas, int cantidad_marcos);
char * dame_mensaje_de_memoria(char **memoria, int nro_marco, int tamanio_marco);
void avisar_a_cpu(char cod_op, char cod_aux, int pid, int paginas, char *mensaje, int socket_cli_cpu);
t_list * inicializar_tlb(int nro_entradas);
int dame_la_direccion_posta_de_la_pagina_en_la_tlb(t_list ** tlb, int pid, int nro_pagina);
char actualizame_la_tlb(t_list ** tlb, int pid, int direccion_posta, int nro_pagina);
void borrame_las_entradas_del_proceso(int pid, t_list ** tlb);
void limpiar_la_tlb(t_list ** tlb);
void limpiar_memoria(t_list ** tabla_de_paginas, char * memoria, int tamanioMarco, int socketSwap);
void volcar_memoria(char * memoria, tconfig_memoria * config, t_log * logMem);
void poneme_en_modificado_la_entrada(tabla_paginas *tabla, int pagina);
int dame_el_numero_de_entrada_de_la_tlb(t_list * tlb, int direccion);

#endif /* SRC_FUNCIONES_MEMORIA_H_ */
