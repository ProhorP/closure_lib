CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -D_DEFAULT_SOURCE -o2

all: test

.PHONY: test

test: main.c
	$(CC) $(CFLAGS) main.c -o $@ 
	./$@

clean:
	rm ./test
