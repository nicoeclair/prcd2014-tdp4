# makefile for standard procedural databases

CC = gcc
HDR = def.h lib.h
LDFLAGS = -lm

all:		tetra tetrahori nff2lanceur

lib.o:		$(HDR) lib.c
		$(CC) -c lib.c

tetra:		lib.o tetra.o
		$(CC) -o tetra tetra.o lib.o $(LDFLAGS)

tetra.o:	$(HDR) tetra.c
		$(CC) -c tetra.c

tetrahori:	lib.o tetrahori.o
		$(CC) -o tetrahori tetrahori.o lib.o $(LDFLAGS)

tetrahori.o:	$(HDR) tetrahori.c
		$(CC) -c tetrahori.c

nff2lanceur : modeleur.h definition.h nff2lanceur.c
	$(CC) nff2lanceur.c -o nff2lanceur

clean:
	rm -f *.o *~
	rm -f tetrahori nff2lanceur tetra
