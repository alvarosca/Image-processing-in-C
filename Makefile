CFLAGS=-Wall -Werror -std=c99 -pedantic -g
LDLIBS=-lm

all: main

main: opcion.o filtro.o imagen.o pixel.o main.o
	gcc $(CFLAGS) opcion.o filtro.o imagen.o pixel.o main.o -o main $(LDLIBS)

main.o: main.c opcion.o filtro.h imagen.h pixel.h
	gcc $(CFLAGS) -c main.c

pixel.o: pixel.c pixel.h
	gcc $(CFLAGS) -c pixel.c

imagen.o: imagen.c imagen.h pixel.h
	gcc $(CFLAGS) -c imagen.c

filtro.o: filtro.c filtro.h imagen.h pixel.h
	gcc $(CFLAGS) -c filtro.c

opcion.o: opcion.c opcion.h filtro.h imagen.h pixel.h
	gcc $(CFLAGS) -c opcion.c


