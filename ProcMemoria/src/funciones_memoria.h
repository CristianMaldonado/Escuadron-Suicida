#ifndef SRC_FUNCIONES_MEMORIA_H_
#define SRC_FUNCIONES_MEMORIA_H_

#include "estructuras.h"
#include <commons/collections/list.h>

char * crear_memoria(int cantidad_marcos, int tamanio_marcos);
tabla_paginas * inicializar_tabla_de_paginas(int cantidad_maxima_marcos_por_proceso, int pid);
void eliminar_tabla_de_proceso(int pid, t_list ** lista_tabla_de_paginas);
tabla_paginas *dame_la_tabla_de_paginas(int pid, t_list ** lista_tabla_de_paginas);
int dame_la_direccion_de_la_pagina(tabla_paginas *tabla, int pagina);


#endif /* SRC_FUNCIONES_MEMORIA_H_ */
