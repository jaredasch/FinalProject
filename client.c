#include "networking.h"
#include <string.h>

void handle_response(char * res){
    printf("%s\n", res);
}

int main(int argc, char **argv) {
    int server_socket;

    if (argc == 2){
      server_socket = client_setup( argv[1]);
    }
    else{
      server_socket = client_setup( "127.0.0.1" );
    }

    while(1){
        char * buffer = calloc(BUFFER_SIZE, sizeof(char *));
        printf("slicky-wiki$ ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = 0;

        write(server_socket, buffer, BUFFER_SIZE);
        read(server_socket, buffer, BUFFER_SIZE);

        handle_response(buffer);
    }
}
