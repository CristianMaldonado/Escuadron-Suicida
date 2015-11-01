# makefile para swap

swap: swap.o config.o libSocket.o manejo_listas_archivo_swap.o paquetes.o
	gcc -o swap swap.o config.o libSocket.o manejo_listas_archivo_swap.o paquetes.o -lcommons

swap.o: swap.c libSocket.h estructuras.h manejode_listas_archivo_swap.h config.h paquetes.h
	gcc -c swap.c -lcommons

config.o: config.c libSocket.h estructuras.h paquetes.h
	gcc -c config.c -lcommons

libSocket.o: libSocket.c
	gcc -c libSocket.c

manejo_listas_archivo_swap.o: estructuras.h paquetes.h
	gcc -c manejo_listas_archivo.c -lcommons

paquetes.o: estructuras.h config.h
	gcc -c paquetes.c
