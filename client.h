#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#ifndef CLIENT_H
#define CLIENT_H

#define RES_EDIT 1
#define RES_DISP 2
#define RES_EXIT 3


struct response {
    int type;
    char body[BUFFER_SIZE - 1];
};

static void sighandler(int signo);
void handle_response(struct response * res, int server_socket);
void no_command();

#endif
