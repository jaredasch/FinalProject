#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
    mkfifo("wkp", 0644);
    printf("(server) Waiting for client connection to WKP\n");
    int wkp = open("wkp", O_RDONLY);

    char private_pipe_name[HANDSHAKE_BUFFER_SIZE];
    printf("(server) Preparing to recieve private pipe name...\n");
    read(wkp, private_pipe_name, HANDSHAKE_BUFFER_SIZE);
    remove("wkp");

    int f = fork();
    if(f) return 0;

    printf("(server) Received private pipe name %s\n", private_pipe_name);
    int downstream = open(private_pipe_name, O_WRONLY);
    write(downstream, ACK, HANDSHAKE_BUFFER_SIZE);
    printf("(server) Sending acknowledgement message\n");

    char msg[HANDSHAKE_BUFFER_SIZE];
    read(wkp, msg, HANDSHAKE_BUFFER_SIZE);
    printf("(server) Received acknowledgement message %s from client\n", msg);
    *to_client = downstream;
    return wkp;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
    printf("(client) Waiting to connect to server on wkp\n");
    int wkp = open("wkp", O_WRONLY);

    char private_pipe_name[HANDSHAKE_BUFFER_SIZE] = "priv_pipe";
    mkfifo(private_pipe_name, 0644);

    printf("(client) Creating private pipe with name %s\n", private_pipe_name);

    write(wkp, private_pipe_name, HANDSHAKE_BUFFER_SIZE);
    printf("(client) Sent private pipe name \"%s\" to server...waiting for server connection to private pipe\n", private_pipe_name);
    int downstream = open(private_pipe_name, O_RDONLY);

    printf("(client) Waiting for acknowledgement from server...\n");
    char msg[HANDSHAKE_BUFFER_SIZE];
    read(downstream, msg, HANDSHAKE_BUFFER_SIZE);
    printf("(client) Received acknowledgement message %s from server\n", msg);

    printf("(client) Sending acknowledgement message %s back to server\n", msg);
    write(wkp, msg, HANDSHAKE_BUFFER_SIZE);

    remove(private_pipe_name);
    *to_server = wkp;
    return downstream;
}
