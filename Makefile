CC=gcc
CFLAGS=-I.

utils: utilities.o
	$(CC) -c -o utilities.o utilities.c