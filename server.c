#include <signal.h>
#include "networking.h"
#include "user.h"

static void sighandler(int signo) {
    if (signo == SIGINT) {
        printf("Removing WKP....");
        remove("wkp");
        printf("Done\n");
        exit(1);
    }
}

char ** parse_args(char * cmd) {
    char ** arg_list = calloc(sizeof(char * ), 25);
    for (int i = 0;
        (arg_list[i] = strsep( & cmd, " ")); i++) {
        // If the arg is an empty string (extra spaces), remove it
        if ( * arg_list[i] == 0) {
            i--;
        }
    }
    return arg_list;
}

void create_page(char * page_name, int client_socket) {
    printf("Trying to create page %s...", page_name);
    char filename[64] = "data/pages/";
    strcat(filename, page_name);
    int fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd == -1) {
        printf("page creation failed\n");
        write(client_socket, "Page already exists", BUFFER_SIZE);
    } else {
        printf("successfully created\n");
        write(client_socket, "Page created successfully", BUFFER_SIZE);
    }
}

void command_handler(char ** args, int client_socket) {
    for(int i = 0; i < strlen(args[0]); i++){
        printf("%c", args[0][i]);
    }
    // printf("args[LAST]: %s\n", args[strlen(args[0])-1]);
    if (strcmp(args[0], "create-page") == 0) {
        create_page(args[1], client_socket);
    } else {
        write(client_socket, args[0], BUFFER_SIZE);
    }
}

void authenticate_user(char ** args, int client_socket, char ** username){
    if (strcmp(args[0], "login") == 0) {
        if ( args[1] && args[2] && validate_user(args[1], args[2])) {
            *username = calloc(1, 64);
            strcpy(*username, args[1]);
            write(client_socket, "Logged in successfully", BUFFER_SIZE);
        } else {
            write(client_socket, "Username or password incorrect", BUFFER_SIZE);
        }
    } else if (strcmp(args[0], "signup") == 0) {
        if ( !(args[1] && args[2]) ){
            write(client_socket, "Command use: 'signup <username> <password>'", BUFFER_SIZE);
        } else if ( add_user(args[1], args[2])) {
            write(client_socket, "User created succesfully", BUFFER_SIZE);
        } else {
            write(client_socket, "Username taken", BUFFER_SIZE);
        }
    } else {
        write(client_socket, "Please login with 'login <username> <password>' or create an account with signup <username> <password>'", BUFFER_SIZE);
    }
}

int main() {
    int listen_socket;

    listen_socket = server_setup();
    signal(SIGINT, sighandler);

    while (1) {
        char * username = NULL;

        int client_socket = server_connect(listen_socket);

        char data[BUFFER_SIZE];
        int f = fork();
        if (f == 0) {
            while (read(client_socket, data, BUFFER_SIZE)) {
                char ** args = parse_args(data);
                if (!username) {
                    authenticate_user(args, client_socket, &username);
                } else {
                    printf("(subserver %d) Recieved \"%s\" from client\n", getpid(), data);
                    command_handler(args, client_socket);
                }
            }
        }
        else{
          close(client_socket);
      }
  }
}
