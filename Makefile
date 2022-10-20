CC=gcc
CFLAGS=-I. -Wall

.PHONY: all
all: main

main: main.c board.c pgnparser.c printing.c
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f main a.out
