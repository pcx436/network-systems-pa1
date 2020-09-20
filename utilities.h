//
// Created by jmalcy on 9/19/20.
//

#ifndef NETWORK_SYSTEMS_PA1_UTILITIES_H
#define NETWORK_SYSTEMS_PA1_UTILITIES_H
#define BUFSIZE 1024

void trimSpace(char *);
void getFile(int sockfd, struct sockaddr_in *serveraddr, int *serverlen, const char *parameter);
void sendFile(int, struct sockaddr_in *, int, const char *);

#endif //NETWORK_SYSTEMS_PA1_UTILITIES_H
