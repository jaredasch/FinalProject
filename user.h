#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>

#ifndef USER_H
#define USER_H

int add_user(char *, char *);
int validate_user(char *, char *);

#endif
