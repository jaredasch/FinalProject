#include <signal.h>
#include <dirent.h>
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

void edit_page(char * page_name, int client_socket){
    struct response * res = calloc(1, sizeof(struct response));
    res->type = RES_EDIT;

    //opens file
    char path[256] = "data/pages/";
    strcat(path, page_name);
    int fd = open(path, O_RDONLY);
    if(fd == -1){
        res->type = RES_DISP;
        strcpy(res->body, "Page doesn't exist");
        write(client_socket, res, BUFFER_SIZE);
        free(res);
    } else {
        char * buffer = calloc(1, BUFFER_SIZE);
        read(fd, buffer, BUFFER_SIZE);

        //copies file into buffer
        strcpy(res->body, buffer);
        close(fd);
        free(buffer);
        write(client_socket, res, BUFFER_SIZE);
        free(res);

        //reads edited file from socket, and modifies file on server
        char * new_file_buffer = calloc(1, BUFFER_SIZE);
        if(read(client_socket, new_file_buffer, BUFFER_SIZE)){
            int edit_fd = open(path, O_WRONLY | O_TRUNC);
            write(edit_fd, new_file_buffer, strlen(new_file_buffer));

            //sends confirmation to client
            struct response * edit_res = calloc(1, sizeof(struct response));
            edit_res->type = RES_DISP;
            strcpy(edit_res->body, "Page edited succesfully");
            write(client_socket, edit_res, BUFFER_SIZE);
            close(edit_fd);
            free(edit_res);
            free(new_file_buffer);
        }
    }
}

void create_page(char * page_name, int client_socket) {
    struct response * res = calloc(1, sizeof(struct response));
    res->type = RES_DISP;

    printf("Trying to create page %s...", page_name);
    char filename[64] = "data/pages/";
    strcat(filename, page_name);

    int fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd == -1) {
        strcpy(res->body, "Page already exists");
        printf("page creation failed\n");
    } else {
        printf("successfully created\n");
        strcpy(res->body, "Page created successfully");
    }
    close(fd);
    free(res);

    edit_page(page_name, client_socket);
}

void get_page(char * page_name, int client_socket){
    struct response * res = calloc(1, sizeof(struct response));
    res->type = RES_DISP;

    char path[256] = "data/pages/";
    strcat(path, page_name);
    int fd = open(path, O_RDONLY);
    if(fd == -1){
        strcpy(res->body, "Page doesn't exist");
        write(client_socket, res, BUFFER_SIZE);
    } else {
        char * buffer = calloc(1, BUFFER_SIZE);
        read(fd, buffer, BUFFER_SIZE);

        strcpy(res->body, buffer);
        close(fd);
        write(client_socket, res, BUFFER_SIZE);
    }
    free(res);
}

void search_titles(char * str, int client_socket){
  DIR* d = opendir("./data/pages");
  int size = strlen(str);
  struct dirent *page;

  while((page = readdir(d))){
    if (page->d_type == DT_REG){
      char * page_name = page->d_name;

      for(int i = 0; i < size; i++){
        if(strcmp(page_name[i],str[i]) == -1){
          return;
        }
      }
    }
  }
}

void command_handler(char ** args, int client_socket) {
    struct response * res = calloc(1, sizeof(struct response));
    res->type = RES_DISP;

    if (strcmp(args[0], "create-page") == 0) {
        create_page(args[1], client_socket);
    } else if(strcmp(args[0], "get-page") == 0){
        get_page(args[1], client_socket);
    } else if(strcmp(args[0], "edit-page") == 0){
        edit_page(args[1], client_socket);
    } else if(strcmp(args[0], "search-titles") == 0){
      search_titles(args[1], client_socket);
    } else {
        printf("Something else: %s\n", args[0]);
        strcpy(res->body, "Command not recognized");
        write(client_socket, res, BUFFER_SIZE);
    }
    free(res);
}

void authenticate_user(char ** args, int client_socket, char ** username){
    struct response * res = calloc(1, sizeof(struct response));
    res->type = RES_DISP;

    if (strcmp(args[0], "login") == 0) {
        if ( args[1] && args[2] && validate_user(args[1], args[2])) {
            *username = calloc(1, 64);
            strcpy(*username, args[1]);
            strcpy(res->body, "Logged in succesfully");
        } else {
            strcpy(res->body, "Username or password incorrect");
        }
    } else if (strcmp(args[0], "signup") == 0) {
        if ( !(args[1] && args[2]) ){
            strcpy(res->body, "Command use: 'signup <username> <password>'");
        } else if ( add_user(args[1], args[2])) {
            strcpy(res->body, "User created succesfully");
        } else {
            strcpy(res->body, "Username taken");
        }
    } else {
        strcpy(res->body, "Please login with 'login <username> <password>' or create an account with signup <username> <password>'");
    }

    write(client_socket, res, BUFFER_SIZE);
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
