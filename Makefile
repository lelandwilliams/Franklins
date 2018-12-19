CC=gcc -std=gnu99 -Wall
CFLAGS=-I. -Wall

all: franklin

network.o: network.h network.c
	$(CC) -c network.c

main.o: state.h network.o main.c
	$(CC) -c main.c 

franklin: network.o main.o
	$(CC) main.o -o franklin

clean:
	rm -v *.o franklin


