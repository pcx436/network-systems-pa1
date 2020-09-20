CC=gcc
CFLAGS=-I. -std=c11

default: client server

client: utils client/udp_client.o
	$(CC) -o client/udpclient client/udp_client.o utilities.o

server: utils server/udp_server.o
	$(CC) -o server/udpserver server/udp_server.o utilities.o

utils: utilities.o
	$(CC) -c -o utilities.o utilities.c

clean:
	rm utilities.o
	rm client/udp_client.o
	rm server/udp_server.o