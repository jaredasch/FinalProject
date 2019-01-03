#include <signal.h>
#include "pipe_networking.h"


static void sighandler(int signo){
    if(signo == SIGINT){
        printf("Removing WKP....");
        remove("wkp");
        printf("Done\n");
        exit(1);
    }
}


char ** parse_args( char * cmd ){
    char ** arg_list = calloc(sizeof(char*), 25);
    for(int i = 0; (arg_list[i] = strsep(&cmd, " ")); i++){
        // If the arg is an empty string (extra spaces), remove it
        if(*arg_list[i] == 0){
            i--;
        }
    }
    return arg_list;
}

void create_page(char * page_name, int to_client){
    printf("Trying to create page %s...", page_name);
    char filename[64] = "data/pages/";
    strcat(filename, page_name);
    int fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if(fd == -1){
	printf("page creation failed\n");
	write(to_client, "Page already exists", 64);
    } else {
	printf("successfully created\n");
    }
}

void command_handler(char * cmd, int to_client){
    char ** args = parse_args(cmd);
    printf("%s\n", args[0]);
    if(strcmp(args[0], "create-page") == 0){
	create_page(args[1], to_client);
    }
}


int main() {
    int to_client;
    int from_client;

    signal(SIGINT, sighandler);

    while(1){
        from_client = server_handshake( &to_client );
        char data[BUFFER_SIZE];
        if(from_client){
            while(read(from_client, data, BUFFER_SIZE)){
	        printf("(subserver %d) Recieved \"%s\" from client\n", getpid(), data);
                command_handler(data, to_client);
		write(to_client, data, BUFFER_SIZE);
            }
        }
    }
}
