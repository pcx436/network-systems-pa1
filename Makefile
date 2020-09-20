CC=gcc
CFLAGS=-I.

default: client server

client: client/udp_client.o utils
	$(CC) -o server/udpclient client/udp_client.o utilities.o

server: server/udp_server.o utils
	$(CC) -o server/udpserver server/udp_server.o utilities.o

utils: utilities.o
	$(CC) -c -o utilities.o utilities.c

clean:
	rm utilities.o
	rm client/udp_client.o
	rm server/udp_server.o