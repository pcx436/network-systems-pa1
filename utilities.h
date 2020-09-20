//
// Created by jmalcy on 9/19/20.
//

#ifndef NETWORK_SYSTEMS_PA1_UTILITIES_H
#define NETWORK_SYSTEMS_PA1_UTILITIES_H
#define BUFSIZE 1024
#include <netinet/in.h>

void trimSpace(char *);
void getFile(int, struct sockaddr_in *, int *, const char *);
void sendFile(int, struct sockaddr_in *, int, const char *);

#endif //NETWORK_SYSTEMS_PA1_UTILITIES_H
