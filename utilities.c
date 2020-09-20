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
#include "utilities.h"

/*
 * Removes trailing spaces from a string.
 */
void trimSpace(char *str){
	int end = strlen(str) - 1;
	while(isspace(str[end])) end--;
	str[end+1] = '\0';
}

int receiveFile(int sockfd, struct sockaddr_in *serveraddr, int *serverlen, const char *parameter) {
	char recv[BUFSIZE];
	int bytesReceived, totalReceived = 0;
	FILE *fileObj = fopen(parameter, "wb");
	bzero(recv, BUFSIZE);

	// exchange file size
	do {
		bytesReceived = recvfrom(sockfd, recv, BUFSIZE, 0, (struct sockaddr *) serveraddr, serverlen);

		if (bytesReceived >= 0) {
			totalReceived += bytesReceived;
			fwrite(recv, sizeof(char), bytesReceived, fileObj);
			bzero(recv, BUFSIZE);

		} else {
			printf("ERROR: %s\n", strerror(errno));
			bytesReceived = BUFSIZE - 1;  // break the loop
			totalReceived = -1;
		}
	} while (bytesReceived == BUFSIZE);

	fclose(fileObj);
	return totalReceived;
}

int sendFile(int sockfd, struct sockaddr_in *clientaddr, int clientlen, const char *parameter) {
	int bytesSent, bytesRead, totalSent = 0;
	char send[BUFSIZE];
	FILE *fileObj = fopen(parameter, "rb");

	// exchange file size
	do {
		bzero(send, BUFSIZE);

		bytesRead = fread(send, sizeof(char), BUFSIZE, fileObj);
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
