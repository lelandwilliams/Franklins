CC=gcc
CFLAGS=-I.

all: franklin

state.o: state.c
	$(CC) -c state.c

franklin: state.o main.c
	$(CC) -o franklin main.c state.o

clean:
	rm *.o franklin


