CC=gcc
CFLAGS=-I.

server: server/udp_server.o utils
	$(CC) -o server/udpserver server/udp_server.o utilities.o

utils: utilities.o
	$(CC) -c -o utilities.o utilities.c