CC=gcc
CFLAGS=-I. -std=c11

default: client server

client: client/udp_client.o utils
	$(CC) -o client/udpclient client/udp_client.o utilities.o

server: server/udp_server.o utils
	$(CC) -o server/udpserver server/udp_server.o utilities.o

utils: utilities.o
	$(CC) -c -o utilities.o utilities.c

clean:
	rm utilities.o
	rm client/udp_client.o
	rm server/udp_server.o