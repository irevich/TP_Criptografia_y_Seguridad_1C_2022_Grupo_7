CC=gcc
CCFLAGS = -Wall -g -lcrypto
LOGFILE := errors.log

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=%.o)

OUTPUT := stegobmp.out
# PARAMS := --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg LSB1
PARAMS := --extract -p resources/ladoLSB4.bmp --out archivoOculto --steg LSB4
all:
	${CC} $(SOURCES) $(CCFLAGS) -o $(OUTPUT)

clean:
	rm -rvf *.o $(OUTPUT)

run: all
	./$(OUTPUT) $(PARAMS) 2> $(LOGFILE)