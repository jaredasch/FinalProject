#include "sem.h"

int create_sem(char* name){
  key_t sem_key = ftok(".", (int) name);

  int semID = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0644); //creates semaphore

  printf("%d\n",semID);

  if(semID == -1){
    printf("ALREADY EXISTS\n");
    return -1;
  }

  union semun data;
  data.val = 1;
  semctl(semID,0,SETVAL,data);
  return 0;
}

int down_sem(char* name){ //returns -1 if semaphore is unavailible, 0 if all clear
  key_t sem_key = ftok(".", (int) name);

  int semID = semget(sem_key, 1, 0); //gets semaphore
  printf("name: %d [%d]\n",(int) name, semID);

  union semun data;

  printf("Semaphore number before downing: %d\n",semctl(semID,0,GETVAL,data));

  struct sembuf opp;
  opp.sem_op = -1;
  opp.sem_flg = SEM_UNDO | IPC_NOWAIT;
  opp.sem_num = 0;
  if(semop(semID,&opp,1)){ //DOWNS the semaphore
    printf("Semaphore number after downing: %d\n",semctl(semID,0,GETVAL,data));
    return -1;
  }
  return 0;
}


void up_sem(char* name){
  key_t sem_key = ftok(".", (int) name);

  int semID = semget(sem_key, 1, 0); //gets semaphore

  struct sembuf opp;
  opp.sem_op = 1;
  opp.sem_flg = SEM_UNDO;
  opp.sem_num = 0;

  union semun data;
  printf("Semaphore number before upping: %d\n",semctl(semID,0,GETVAL,data));

  semop(semID,&opp,1); //UPS the semaphore

  printf("Semaphore number after upping: %d\n",semctl(semID,0,GETVAL,data));
}

void rm_sem(char* name){
  key_t sem_key = ftok(".", (int) name);

  int semID = semget(sem_key, 1, 0); //gets semaphore
  semctl(semID,0,IPC_RMID,0); //removes semaphore
}
