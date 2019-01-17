#include <signal.h>
#include <dirent.h>
#include "networking.h"
#include "user.h"

int client_socket;

static void sighandler(int signo) {
    if (signo == SIGINT) {
        close(client_socket);
        exit(1);
    }
}

char ** parse_args(char * cmd) {
    char ** arg_list = calloc(sizeof(char * ), 25);
    for (int i = 0; (arg_list[i] = strsep( & cmd, " ")); i++) {
        // If the arg is an empty string (extra spaces), remove it
        if ( * arg_list[i] == 0) {
            i--;
        }
    }
    return arg_list;
}

void edit_page(char * page_name){
    struct response * res = calloc(1, sizeof(struct response));
    res->type = RES_EDIT;

    if(page_name[0] == '.'){
        res->type = RES_DISP;
        strcpy(res->body, "Invalid file path");
        write(client_socket, res, BUFFER_SIZE);
        free(res);
        return;
    }

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

void create_page(char * page_name) {
    struct response * res = calloc(1, sizeof(struct response));
    res->type = RES_DISP;

    if(page_name[0] == '.'){
        strcpy(res->body, "Invalid file path");
        write(client_socket, res, BUFFER_SIZE);
        free(res);
        return;
    }

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

    edit_page(page_name);
}

void get_page(char * page_name){
    struct response * res = calloc(1, sizeof(struct response));
    res->type = RES_DISP;

    if(page_name[0] == '.'){
        res->type = RES_DISP;
        strcpy(res->body, "Invalid file path");
        write(client_socket, res, BUFFER_SIZE);
        free(res);
        return;
    }

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

void search_titles(char * str){
  DIR* d = opendir("data/pages");
  int size = strlen(str);
  struct dirent *page;
  int count = 0; //number of hits
  char ** ans = calloc(50, sizeof(char *));
  char * page_name = calloc(256, 1);

  while((page = readdir(d))){
    if (page->d_type == DT_REG){ //if item is a file
      char * page_name_cpy = page->d_name;
      strcpy(page_name, page->d_name);
      page_name[size] = 0;
      if (strcmp(page_name,str) == 0){
        ans[count] = page_name_cpy;
        count++;
      }
    }
  }
  closedir(d);
  struct response * res = calloc(1, sizeof(struct response));
  res->type = RES_DISP;
  strcat(res->body,"----[");
  char num[50];
  sprintf(num, "%d", count);
  strcat(res->body,num);
  strcat(res->body,"] Hits Found----\n");

  int i = 0;
  while(ans[i]){
    strcat(res->body, ans[i]);
    strcat(res->body, "\n");
    i++;
  }
  strcat(res->body,"----------------------\n");
  write(client_socket, res, BUFFER_SIZE);
  free(ans);
  free(page_name);
  free(res);
}

void search_contents(char * str){
  DIR* d = opendir("data/pages");
  struct dirent *page;
  char * page_name = calloc(256, 1);
  int count; //number of hits
  struct response * res = calloc(1, sizeof(struct response));
  res->type = RES_DISP;
  strcat(res->body,"----------------------\n");

  while((page = readdir(d))){
    if (page->d_type == DT_REG){ //if item is a file
      strcpy(page_name, page->d_name);

      char * path = calloc(256, 1);
      strcat(path,"data/pages/");
      strcat(path,page_name);

      printf("[%s]\n",page_name);
      int fd = open(path, O_RDONLY);

      char * page_contents = calloc(1, BUFFER_SIZE);
      int n = read(fd, page_contents, BUFFER_SIZE);
      //printf("open: %d read bytes: %d\n",fd,n);

      count = 0;
      while ((page_contents = strstr(page_contents,str))){
        page_contents[0] = *(str)+1;
        count++;
      }
      if(count>0){
        char num[50];
        sprintf(num, "%d", count);
        strcat(res->body,num);
        strcat(res->body," Hit(s) found in page '");
        strcat(res->body, page_name);
        strcat(res->body, "'\n");
        close(fd);
        }
      }
    }
    closedir(d);
    //printf("writing [%s] \n",res->body);
    strcat(res->body,"----------------------\n");
    write(client_socket, res, BUFFER_SIZE);
    free(page_name);
    free(res);
}

void show_pages(){
  DIR* d = opendir("data/pages");
  struct dirent *page;
  char * buffer = calloc(BUFFER_SIZE, sizeof(char *));
  strcat(buffer,"----Pages----\n");
  char * page_name = calloc(256, 1);

  while((page = readdir(d))){
    if (page->d_type == DT_REG){ //if item is a file
      strcpy(page_name,page->d_name);
      char * temp = page->d_name;
      page_name[1] = 0;
      //printf("%s\n",temp);
      if(strcmp(page_name,".") != 0){
        strcat(buffer,temp);
        strcat(buffer,"\n");
      }
    }
  }
  closedir(d);
  free(page_name);
  buffer[strlen(buffer)-1] = 0;
  struct response * res = calloc(1, sizeof(struct response));
  res->type = RES_DISP;
  strcpy(res->body,buffer);
  strcat(res->body,"\n--------------\n");
  write(client_socket, res, BUFFER_SIZE);
  free(res);
}

void server_exit(){
  struct response * res = calloc(1, sizeof(struct response));
  res->type = RES_EXIT;
  write(client_socket,res,BUFFER_SIZE);
  close(client_socket);
  printf("(subserver %d) exiting\n", getpid());
  free(res);
  exit(0);
}


void command_handler(char ** args) {
    struct response * res = calloc(1, sizeof(struct response));
    res->type = RES_DISP;

    if (strcmp(args[0], "create-page") == 0 && args[1]) {
        create_page(args[1]);
    } else if(strcmp(args[0], "get-page") == 0 && args[1]){
        get_page(args[1]);
    } else if(strcmp(args[0], "edit-page") == 0 && args[1]){
        edit_page(args[1]);
    } else if(strcmp(args[0], "search-titles") == 0 && args[1]){
      search_titles(args[1]);
    } else if(strcmp(args[0], "search-contents") == 0 && args[1]){
      search_contents(args[1]);
    } else if(strcmp(args[0], "show-pages") == 0){
        show_pages();
    } else {
        printf("(subserver %d) something else: %s\n",getpid(), args[0]);
        strcpy(res->body, "Command not recognized");
        write(client_socket, res, BUFFER_SIZE);
    }
    free(res);
}

void authenticate_user(char ** args, char ** username){
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
    free(res);
}

void no_command(){
  struct response * res = calloc(1, sizeof(struct response));
  res->type = RES_DISP;
  strcpy(res->body, "Please enter a command\n");
  write(client_socket, res, BUFFER_SIZE);
  free(res);
}


int main() {
    int listen_socket;

    listen_socket = server_setup();
    signal(SIGINT, sighandler);

    while (1) {
        char * username = NULL;

        client_socket = server_connect(listen_socket);

        int f = fork();
        if (f == 0) { //subserver
            printf("(subserver %d) forked\n", getpid());
            char * data = calloc(1, BUFFER_SIZE);
            while (read(client_socket, data, BUFFER_SIZE)) {
                printf("(subserver %d) Recieved \"%s\" from client\n", getpid(), data);

                if(strlen(data) == 0){ //if only a \n was sent through socket
                  no_command();
                }
                else{
                  char ** args = parse_args(data);

                  if(strcmp(args[0], "exit") == 0){ //checks if user wants to exit
                    printf("(subserver %d) exiting\n", getpid());
                    server_exit();
                  }
                  if (!username) { //makes sure user logs in
                      authenticate_user(args, &username);
                    }
                  else {
                    command_handler(args);
                    }
                }
            } //end while loop
            close(client_socket);
            printf("(subserver %d) closed socket\n", getpid());
            exit(0);
        } //end of subserver
        else{
          close(client_socket);
      }
  }
}
