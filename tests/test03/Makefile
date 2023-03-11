CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -std=gnu99 -g

all: test

.PHONY: test

test: main.c
	$(CC) $(CFLAGS) main.c -o $@ 
	./$@

clean:
	rm ./test
