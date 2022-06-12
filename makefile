CC=gcc
CCFLAGS = -Wall -g

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=%.o)

OUTPUT := stegobmp.out
PARAMS := --embed --in resources/secret.txt --p resources/lado.bmp --out output.bmp --steg LSB1
all:
	${CC} $(SOURCES) $(CCFLAGS) -o $(OUTPUT)

clean:
	rm -rvf *.o OUTPUT

run: all
	./$(OUTPUT) $(PARAMS)