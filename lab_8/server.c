#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define bufSize 128
#define semName "/sem3"

int shmid;
char* segptr;
sem_t* sem;

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

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(1);
    }

    if (sem_close(sem) == -1)
    {
        perror("sem_close");
        exit(1);
    }

    if (sem_unlink(semName) == -1)
    {
        perror("sem_unlink");
        exit(1);
    }

    exit(0);
}

int main()
{
    signal(SIGINT, signalfunction);

    char buffer[bufSize];
    key_t key = ftok(".", 'S');
    
    if ((sem = sem_open(semName, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    if ((shmid = shmget(key, bufSize, IPC_CREAT | IPC_EXCL | 0666)) == -1)
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
        sleep(1);
        struct tm* curTime = getTime();
        char pid[16];

        sprintf(pid, "%d", getpid());

        strcpy(buffer, asctime(curTime));
        buffer[strlen(buffer) - 1] = ' ';
        strcat(buffer, "    sender pid = ");
        strcat(buffer, pid);

        sem_wait(sem);
        strcpy(segptr, buffer);
        sem_post(sem);
    }

    return(0);
}