CC=gcc
CFLAGS=-I. -Wall

all: franklin

network.o: network.h network.c
	$(CC) -c network.c

main.o: state.h network.h main.c
	$(CC) -c main.c 

franklin: network.o main.o
	$(CC) main.o -o franklin

clean:
	rm *.o franklin


