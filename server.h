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
#include <dirent.h>

#ifndef SERVER_H
#define SERVER_H

#define RES_EDIT 1
#define RES_DISP 2
#define RES_EXIT 3


struct response {
    int type;
    char body[BUFFER_SIZE - 1];
};

static void sighandler(int signo);
char ** parse_args(char * cmd);x
void command_handler(char ** args);
void authenticate_user(char ** args, char ** username);

void edit_page(char * page_name);
void create_page(char * page_name);
void get_page(char * page_name);
void search_titles(char * str);
void search_contents(char * str);
void show_pages();
void server_exit();

#endif
