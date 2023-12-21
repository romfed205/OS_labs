#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>


void _print(){
    printf("Program exited.\n");
}

void _sigint(int sig){
    printf("Received SIGINT signal (%d)\n", sig);
	exit(0);
}

void _sigterm(int sig, siginfo_t *info, void *context){
    printf("Received SIGTERM signal. Signal number: %d\n", sig);
	printf("Signal description: %s\n", strsignal(sig));
	printf("Sender Process ID: %d\n", info->si_pid);

    exit(0);
}

int main(int argc, char* argv[])
{
    pid_t pid = fork();

	int status;
	atexit(_print);
	signal(SIGINT, _sigint);
	struct sigaction sigterm_action;
    sigterm_action.sa_sigaction = _sigterm;
    sigemptyset(&sigterm_action.sa_mask);
    sigterm_action.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &sigterm_action, NULL);
        if(pid < 0){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        else if(pid == 0){
        	printf("Child process. Process ID: %d\n", getpid());
        	sleep(5);
        	printf("Child process completed.\n");
        	exit(0); 
        }

        else{
        	printf("Parent process. Process ID: %d, Child ID: %d\n", getpid(), pid);
            waitpid(pid, &status, WNOHANG);
        	printf("Child process finished. Status: %d\n", status);
        }
        
    return 0;
}
