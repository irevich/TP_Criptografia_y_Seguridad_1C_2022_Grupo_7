CC=gcc
CCFLAGS = -Wall -g

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=%.o)

OUTPUT := stegobmp

all:
	${CC} $(SOURCES) $(CCFLAGS) -o $(OUTPUT)

clean:
	rm -rvf *.o OUTPUT