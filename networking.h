#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef NETWORKING_H
#define NETWORKING_H

#define PORT "9001"
#define BUFFER_SIZE 1024

int server_setup();
int server_connect(int sd);
int client_setup(char * server);


#endif
