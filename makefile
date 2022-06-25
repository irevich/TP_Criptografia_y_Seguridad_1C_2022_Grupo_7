CC=gcc
CCFLAGS = -Wall -g -lcrypto
LOGFILE := errors.log

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=%.o)

OUTPUT := stegobmp.out
#PARAMS := --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg LSB1
PARAMS := --extract -p resources/ladoLSBI.bmp --out archivoOculto --steg LSBI
all:
	${CC} $(SOURCES) $(CCFLAGS) -o $(OUTPUT)

clean:
	rm -rvf *.o $(OUTPUT)
	rm $(LOGFILE)
	rm *.bmp

full: clean all

run: all
	./$(OUTPUT) $(PARAMS) 2> $(LOGFILE)