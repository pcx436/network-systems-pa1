/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../utilities.h"

void ls(int sockfd, struct sockaddr_in *serveraddr, int *serverlen) {
	char recv[BUFSIZE];
	int bytesReceived;

	// exchange file size
	do {
		bzero(recv, BUFSIZE);
		bytesReceived = recvfrom(sockfd, recv, BUFSIZE, 0, (struct sockaddr *) serveraddr, serverlen);

		if (strlen(recv) > 0) {
			printf("%s\n", recv);
		} else if (strlen(recv) == 0) {  // received final, empty string
			bytesReceived = BUFSIZE - 1;
		} else {
			printf("ERROR: %s\n", strerror(errno));
			bytesReceived = BUFSIZE - 1;  // break the loop
		}
	} while (bytesReceived == BUFSIZE);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE], recv[BUFSIZE], *command, *parameter;

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
	// IPv4 UDP connection
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0],
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    bzero(buf, BUFSIZE);
    bzero(recv, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);

	trimSpace(buf);  // remove \n that may be on the buffer

	/* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf), 0, (const struct sockaddr *) &serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");

    // split the buffer into command and parameter to decide on action
	command = strtok(buf, " ");
	parameter = strtok(NULL, " ");

	// get file
	if (strcmp("get", command) == 0 && parameter != NULL) {
		receiveFile(sockfd, &serveraddr, &serverlen, parameter);
	} else if (strcmp("put", command) == 0 && parameter != NULL) {
		// put file
		sendFile(sockfd, &serveraddr, serverlen,  parameter);
	}

	/* print the server's reply */
	n = recvfrom(sockfd, recv, BUFSIZE, 0, (struct sockaddr *) &serveraddr, &serverlen);
	if (n < 0)
		error("ERROR in recvfrom");
	printf("Echo from server: %s\n", recv);

    return 0;
}
