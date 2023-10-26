#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void atexit_handler(void) {
    printf("Обработчик atexit() вызван\n");
}

void sigint_handler(int signum) {
    printf("Получен сигнал SIGINT (%d)\n", signum);
}

void sigterm_handler(int signum, siginfo_t *info, void *ptr) {
    printf("Получен сигнал SIGTERM (%d)\n", signum);
}

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        printf("Ошибка при вызове fork()\n");
        return 1;
    }

    if (pid == 0) {
        printf("Это процесс-потомок\n");
    } else {
        printf("Это процесс-родитель\n");
    }

    atexit(atexit_handler);

    signal(SIGINT, sigint_handler);

    struct sigaction act;
    act.sa_sigaction = sigterm_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);

    while (1) {
        sleep(1);
    }

    return 0;
}