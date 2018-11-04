CC=gcc
CFLAGS=-I. -Wall

all: franklin

#state.o: state.c
#	$(CC) -c state.c

main.o: state.h main.c
	$(CC) -c main.c 

franklin: state.h main.o
	$(CC) main.o -o franklin

clean:
	rm *.o franklin


