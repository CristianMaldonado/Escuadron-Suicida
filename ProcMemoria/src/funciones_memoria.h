#ifndef SRC_FUNCIONES_MEMORIA_H_
#define SRC_FUNCIONES_MEMORIA_H_

#include "estructuras.h"
#include <commons/collections/list.h>

char * crear_memoria(int cantidad_marcos, int tamanio_marcos);
tabla_paginas * inicializar_tabla_de_paginas(int cantidad_maxima_marcos_por_proceso, int pid);
void eliminar_tabla_de_proceso(int pid, t_list ** lista_tabla_de_paginas);
tabla_paginas *dame_la_tabla_de_paginas(int pid, t_list ** lista_tabla_de_paginas);
int dame_la_direccion_de_la_pagina(tabla_paginas *tabla, int pagina);
bool estan_los_frames_ocupados(t_list *tabla_paginas);
int dame_un_marco_libre(t_list *lista_tabla_de_paginas, int cantidad_marcos);
char * dame_mensaje_de_memoria(char **memoria, int nro_marco, int tamanio_marco);
void avisar_a_cpu(char cod_op, char cod_aux, int pid, int paginas, char *mensaje, int socket_cli_cpu);

#endif /* SRC_FUNCIONES_MEMORIA_H_ */
