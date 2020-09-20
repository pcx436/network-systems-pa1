//
// Created by jmalcy on 9/19/20.
//

#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "utilities.h"

/**
 * Removes trailing spaces from a string.
 * @param str The string to trim space from.
 */
void trimSpace(char *str){
	int end = strlen(str) - 1;
	while(isspace(str[end])) end--;
	str[end+1] = '\0';
}

/**
 * Receives a file over multiple calls to recvfrom and saves it to the provided filename.
 * @param 	sockfd 		The socket file descriptor.
 * @param 	serveraddr 	The server's address information.
 * @param 	serverlen 	The lenght of the server address.
 * @param 	filename 	The name of the file we want to receive.
 * @return 	Returns the number of bytes received upon success or -1 upon error.
 */
int receiveFile(int sockfd, struct sockaddr_in *serveraddr, int *serverlen, const char *filename) {
	char recv[BUFSIZE];
	int bytesReceived, totalReceived = 0;
	FILE *fileObj = fopen(filename, "wb");

	do {
		bzero(recv, BUFSIZE);
		bytesReceived = recvfrom(sockfd, recv, BUFSIZE, 0, (struct sockaddr *) serveraddr, serverlen);

		if (bytesReceived >= 0) {
			totalReceived += bytesReceived;
			fwrite(recv, sizeof(char), bytesReceived, fileObj);
		} else {
			printf("ERROR: %s\n", strerror(errno));
			bytesReceived = BUFSIZE - 1;  // break the loop
			totalReceived = -1;
		}
	} while (bytesReceived == BUFSIZE);  // loop as long as the buffer is full when sent.

	fclose(fileObj);
	return totalReceived;
}

/*
 * Reads in a file specified by the filename parameter and sends it to the specified socket address.
 * Returns the number of bytes sent upon success or -1 upon error.
 */
int sendFile(int sockfd, struct sockaddr_in *clientaddr, int clientlen, const char *filename) {
	int bytesSent, bytesRead, totalSent = 0;
	char send[BUFSIZE];
	FILE *fileObj = fopen(filename, "rb");

	// exchange file size
	do {
		bzero(send, BUFSIZE);

		bytesRead = fread(send, sizeof(char), BUFSIZE, fileObj);  // read BUFSIZE bytes from file
		bytesSent = sendto(sockfd, send, bytesRead, 0, (const struct sockaddr *) clientaddr, clientlen);
		if (bytesSent < 0) {
			printf("ERROR: %s\n", strerror(errno));
			bytesRead = BUFSIZE - 1;
			totalSent = -1;
		} else {
			totalSent += bytesSent;
		}

	} while (bytesRead == BUFSIZE);

	fclose(fileObj);
	return totalSent;
}

/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(0);
}
