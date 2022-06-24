CC=gcc
CCFLAGS = -Wall -g -lcrypto

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=%.o)

OUTPUT := stegobmp.out
PARAMS := --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg LSB1
all:
	${CC} $(SOURCES) $(CCFLAGS) -o $(OUTPUT)

clean:
	rm -rvf *.o $(OUTPUT)

run: all
	./$(OUTPUT) $(PARAMS)