//
// Created by jmalcy on 9/19/20.
//

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "utilities.h"


void trimSpace(char *str){
	int end = strlen(str) - 1;
	while(isspace(str[end])) end--;
	str[end+1] = '\0';
}

void getFile(int sockfd, struct sockaddr_in *serveraddr, int *serverlen, const char *parameter) {
	FILE *fileObj = fopen(parameter, "wb");
	char recv[BUFSIZE];
	bzero(recv, BUFSIZE);
	int n;

	// exchange file size
	do {
		n = recvfrom(sockfd, recv, BUFSIZE, 0, (struct sockaddr *) serveraddr, serverlen);
		if (n >= 0) {
			printf("received \"%s\" (%d) from server\n", recv, n);
			fwrite(recv, sizeof(char), n, fileObj);
			bzero(recv, BUFSIZE);

		} else {
			printf("ERROR: %s\n", strerror(errno));
			n = BUFSIZE - 1;  // break the loop
		}
	} while (n == BUFSIZE);
	printf("Finished loop\n");

	fclose(fileObj);
}

void sendFile(int sockfd, struct sockaddr_in *clientaddr, int clientlen, const char *parameter) {
	int bytesSent, n;
	char send[BUFSIZE];
	FILE *fileObj = fopen(parameter, "rb");

	// exchange file size
	do {
		bzero(send, BUFSIZE);

		n = fread(send, sizeof(char), BUFSIZE, fileObj);
		bytesSent = sendto(sockfd, send, n, 0, (const struct sockaddr *) clientaddr, clientlen);
		if (bytesSent < 0) {
			printf("ERROR: %s\n", strerror(errno));
			n = 123123;
		}
		else {
			printf("sent \"%s\" (%d) from server\n", send, bytesSent);
		}

	} while (n == BUFSIZE);
	printf("Finished loop\n");

	fclose(fileObj);
}