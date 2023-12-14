#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>

#define bufSize 128

int shmid, cntr;
char* segptr;

struct tm* getTime()
{
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return timeinfo;
}

void signalfunction(int sig)
{
    if (shmdt(segptr) == -1)
    {
        perror("shmdt");
        exit(1);
    }
    
    exit(0);
}

int main()
{
    signal(SIGINT, signalfunction);

    char buffer[bufSize];
    key_t key;

    key = ftok(".", 'S');

    if ((shmid = shmget(key, bufSize, 0)) == -1)
    {
        perror("shmget");
        exit(1);
    }

    if ((segptr = shmat(shmid, 0, 0)) == (void*) -1)
    {
        perror("shmat");
        exit(1);
    }

    while(1)
    {
        sleep(3);

        struct tm* parentTime;
        struct tm* curTime = getTime();

        strcpy(buffer, segptr);

        printf("\nData from parent process: %s\n", buffer);
        printf("Time in child process: %s\n", asctime(curTime));
    }

    shmdt(segptr);
    shmctl(shmid, IPC_RMID, NULL);

    return(0);
}