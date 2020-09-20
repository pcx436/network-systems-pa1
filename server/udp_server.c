/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include "../utilities.h"
#include <errno.h>

/**
 * Sends the name of each file in the current directory to the specified address. Each file name is sent individually.
 * @param 	sockfd		The socket file descriptor
 * @param 	clientaddr 	The client address information
 * @param 	clientlen 	The length of the client address
 */
void ls(int sockfd, struct sockaddr_in *clientaddr, int clientlen) {
	DIR *dirptr;
	struct dirent *ent;
	int bytesSent = 0;
	char fileName[BUFSIZE];

	dirptr = opendir("./");

	// exchange file size
	if (dirptr != NULL) {
		// while there are files in the directory, send them to the client
		while (bytesSent >= 0 && (ent = readdir(dirptr)) != NULL) {
			bzero(fileName, BUFSIZE);
			strcpy(fileName, ent->d_name);

			bytesSent = sendto(sockfd, fileName, BUFSIZE, 0,
					  (const struct sockaddr *) clientaddr, clientlen);

			if (bytesSent < 0) {
				printf("ERROR: %s\n", strerror(errno));
			}
		}
	} else {
		// communicate the failure to the client
		strcpy(fileName, strerror(errno));

		// communicate error before blank send
		bytesSent = sendto(sockfd, fileName, BUFSIZE, 0,
					 (const struct sockaddr *) clientaddr, clientlen);

		if (bytesSent < 0) {
			printf("ERROR: %s\n", strerror(errno));
		}
	}
	closedir(dirptr);

	bzero(fileName, BUFSIZE);  // send over an empty buffer to signal finished
	bytesSent = sendto(sockfd, fileName, BUFSIZE, 0, (const struct sockaddr *) clientaddr, clientlen);
	if (bytesSent < 0) {
		printf("ERROR: %s\n", strerror(errno));
	}
}

int main(int argc, char **argv) {
	int sockfd; /* socket */
	int portno; /* port to listen on */
	int clientlen; /* byte size of client's address */
	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in clientaddr; /* client addr */
	struct hostent *hostp; /* client host info */
	char buf[BUFSIZE], response[BUFSIZE], bufBackup[BUFSIZE]; /* message buf */
	char *hostaddrp, *command, *parameter; /* dotted decimal host addr string */
	int optval; /* flag value for setsockopt */
	int n, keepRunning = 1; /* message byte size */

	/*
	 * check command line arguments
	 */
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	portno = atoi(argv[1]);

	/*
	 * socket: create the parent socket
	 */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	/* setsockopt: Handy debugging trick that lets
	 * us rerun the server immediately after we kill it;
	 * otherwise we have to wait about 20 secs.
	 * Eliminates "ERROR on binding: Address already in use" error.
	 */
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
	           (const void *) &optval, sizeof(int));

	/*
	 * build the server's Internet address
	 */
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short) portno);

	/*
	 * bind: associate the parent socket with a port
	 */
	if (bind(sockfd, (struct sockaddr *) &serveraddr,
	         sizeof(serveraddr)) < 0)
		error("ERROR on binding");

	/*
	 * main loop: wait for a datagram, then echo it
	 */
	clientlen = sizeof(clientaddr);
	while (keepRunning == 1) {  // will turn to 0 upon exit command

		/*
		 * recvfrom: receive a UDP datagram from a client
		 */
		bzero(buf, BUFSIZE);
		n = recvfrom(sockfd, buf, BUFSIZE, 0,
		             (struct sockaddr *) &clientaddr, &clientlen);
		if (n < 0)
			error("ERROR in recvfrom");

		/*
		 * gethostbyaddr: determine who sent the datagram
		 */
		hostp = gethostbyaddr((const char *) &clientaddr.sin_addr.s_addr,
		                      sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		if (hostp == NULL)
			error("ERROR on gethostbyaddr");
		hostaddrp = inet_ntoa(clientaddr.sin_addr);
		if (hostaddrp == NULL)
			error("ERROR on inet_ntoa\n");
		printf("server received datagram from %s (%s)\n",
		       hostp->h_name, hostaddrp);
		printf("server received %ld/%d bytes: %s\n", strlen(buf), n, buf);

		/** options are:
		 * get FILE
		 * put FILE
		 * delete FILE
		 * ls
		 * exit
		 * unknown: repeat back to client
		 */
		memcpy(bufBackup, buf, BUFSIZE);  // backup the buffer before it gets mangled by strtok

		// remove \n that may be on buffer
		trimSpace(buf);

		// separate the buffer such that we can tell the difference between the command and parameter (should one exist)
		command = strtok(buf, " ");
		parameter = strtok(NULL, " ");
		bzero(response, BUFSIZE);

		if (strcmp("get", command) == 0 && parameter != NULL) {
			// Communicate success or failure to client depending on return value
			if (sendFile(sockfd, &clientaddr, clientlen, parameter) >= 0 ) {
				sprintf(response, "File \"%s\" transferred successfully!", parameter);
			} else {
				sprintf(response, "File \"%s\" failed to transfer, %s", parameter, strerror(errno));
			}
		} else if (strcmp("put", command) == 0 && parameter != NULL) {
			// Communicate success or failure to client depending on return value
			if (receiveFile(sockfd, &clientaddr, &clientlen, parameter) >= 0) {
				sprintf(response, "File \"%s\" received successfully!", parameter);
			} else {
				sprintf(response, "File \"%s\" could not be received, %s", parameter, strerror(errno));
			}
		} else if (strcmp("delete", command) == 0 && parameter != NULL) {
			// Communicate success or failure to client depending on return value
			if (remove(parameter) == 0) {
				sprintf(response, "File \"%s\" deleted successfully!", parameter);
			} else {
				sprintf(response, "File \"%s\" could not be deleted, error \"%s\".", parameter, strerror(errno));
			}
		} else if (strcmp("ls", command) == 0) {
			// If there was an error, the errno explanation will be in response
			ls(sockfd, &clientaddr, clientlen);
			sprintf(response, "ls command completed.");
		} else if (strcmp("exit", command) == 0) {
			sprintf(response, "Shutting down...");
			keepRunning = 0;
		} else {
			strcpy(response, bufBackup);  // restore original value of buffer to be sent back to client
		}

		// If there is a response to send (always should be), send it to the client.
		if (strlen(response) > 0) {
			/*
			 * sendto: echo the input back to the client
			 */
			n = sendto(sockfd, response, strlen(response), 0,
			           (const struct sockaddr *) &clientaddr, clientlen);
			if (n < 0)
				error("ERROR in sendto");
		}
	}
	close(sockfd);
}
