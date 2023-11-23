#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

static const char* FIFO_PATH = "myfifo";
size_t MSG_SIZE = 100;

int main() {
    pid_t pid;
    char message[MSG_SIZE];
    time_t current_time;
    int fd;

    
    mkfifo(FIFO_PATH, 0666);

    pid = fork();

    if (pid == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        fd = open(FIFO_PATH, O_RDONLY);
        if (fd == -1) {
            perror("Error");
            exit(EXIT_FAILURE);
        }

        
        if (read(fd, message, MSG_SIZE) == -1) {
            perror("FIFO read error");
            exit(EXIT_FAILURE);
        }

        
        current_time = time(NULL);
        
        
        printf("Дочерний процесс (pid=%d) время: %s Родитель: %s\n", getpid(), asctime(localtime(&current_time)), message);
        
        close(fd);
    } 
    
    else {
        fd = open(FIFO_PATH, O_WRONLY);
        if (fd == -1) {
            perror("FIFO write error");
            exit(EXIT_FAILURE);
        }

        current_time = time(NULL);

        
        snprintf(message, MSG_SIZE, "Время: %s PID родителя: %d", asctime(localtime(&current_time)), getpid());

        sleep(5);

        
        if (write(fd, message, strlen(message) + 1) == -1) {
            perror("FIFO write error");
            exit(EXIT_FAILURE);
        }
        
        close(fd);
        
        wait(NULL);
    }

    return 0;
}
