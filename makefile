CC=gcc
CCFLAGS = -Wall -g -lcrypto
LOGFILE := errors.log

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=%.o)

OUTPUT := stegobmp
PARAMS := --embed --in resources/respuesta.png -p resources/lado.bmp --out ladoModificado.bmp --steg LSBI
# PARAMS := --extract -p ./resources/ladoLSB1aes192cbc.bmp --out archivoOculto --steg LSB1 -a AES192 -m CBC -k "escondite"
# PARAMS := --extract -p ./resources/ladoLSB1.bmp --out archivoOculto --steg LSB1
all:
	${CC} $(SOURCES) $(CCFLAGS) -o $(OUTPUT)

clean:
	rm -rvf *.o $(OUTPUT)
	rm $(LOGFILE)
	rm *.bmp

full: clean all

run: all
	./$(OUTPUT) $(PARAMS) 2> $(LOGFILE)
