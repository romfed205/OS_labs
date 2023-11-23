#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define MSG_SIZE 100

int main() {
    int pipefd[2];
    pid_t pid;
    char message[MSG_SIZE];
    time_t current_time;

    
    if (pipe(pipefd) == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]);

        
        if (read(pipefd[0], message, MSG_SIZE) == -1) {
            perror("Pipe read error");
            exit(EXIT_FAILURE);
        }

        
        current_time = time(NULL);
        
        
        printf("Дочерний процесс (pid=%d) время: %s Родитель: %s\n", getpid(), asctime(localtime(&current_time)), message);
        
        close(pipefd[0]);
    }
    
    else {
        close(pipefd[0]);

        current_time = time(NULL);

        snprintf(message, MSG_SIZE, "Время: %s PID родителя: %d", asctime(localtime(&current_time)), getpid());

        sleep(5);

        if (write(pipefd[1], message, MSG_SIZE) == -1) {
            perror("Pipe write error");
            exit(EXIT_FAILURE);
        }
        
        close(pipefd[1]);
        
        wait(NULL);
    }

    return 0;
}
