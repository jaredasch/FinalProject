#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

struct user {
    char username[64];
    char password[64];
};


int add_user(char * username, char * password){
    struct user new_user;
    strcpy(new_user.username, username);
    strcpy(new_user.password, password);

    char path[64] = "data/users/";
    strcat(path, username);

    int f = open(path, O_CREAT | O_WRONLY | O_EXCL, 0666);
    if(f == -1){
        printf("%s\n", strerror(errno));
        return 0;
    } else {
        write(f, &new_user, sizeof(struct user));
        close(f);
        return 1;
    }
}

int validate_user(char * username, char * password){
    char path[64] = "data/users/";
    strcat(path, username);

    int f = open(path, O_RDONLY, 0666);
    if(f == -1){
        printf("%s\n", strerror(errno));
        return 0;
    } else {
        struct user u;
        read(f, &u, sizeof(struct user));
        close(f);
        return !strcmp(u.password, password);
    }
}

void print_user(struct user u){
    printf("%s : %s\n", u.username, u.password);
}
