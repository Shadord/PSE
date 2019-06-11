# PROJET : Fichier Makefile
#
include ../Makefile.inc

EXE = cartes

${EXE): ${PSE_LIB}

all: ${EXE}

clean:
	rm -f *.o *~ ${EXE}
