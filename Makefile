CC=gcc
CFLAGS=-c -Wall -Wextra -Wpedantic -std=c11 -std=gnu99 -g

all: closure

closure: main.o
	$(CC) main.o -o closure

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o closure
