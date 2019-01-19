#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

#ifndef SEM_H
#define SEM_H

int create_sem(char* name);
int down_sem(char* name);
void up_sem(char* name);
void rm_sem(char* name);

union semun{
	int val; //value for SETVAL
	struct semid_ds *buf; //buffer for IPC_STAT & IPC_SET
	unsigned short *array; //array for GETALL/SETALL
	struct seminfo *__buff; //buffer for IPC_INFO(linux specific)
};

#endif
