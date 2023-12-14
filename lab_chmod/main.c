#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

void changeFileMode(char* filePath, char* command)
{
    if (command[0] >= '0' && command[0] <= '7')
    {
        for (unsigned long i = 0; i < strlen(command); ++i)
        {
            if (command[0] < '0' && command[0] > '7')
            {
                exit(1);
            }
        }

        mode_t mode = strtol(command, NULL, 8);

        if (chmod(filePath, mode) < 0)
        {
            exit(1);
        }
    }
    else
    {
        bool user = false;
        bool group = false;
        bool other = false;

        bool read = false;
        bool write = false;
        bool execute = false;

        bool add = false;
        bool remove = false;

        struct stat stats;

        if (command[0] == '+' || command[0] == '-')
        {
            user = true;
            group = true;
            other = true;
        }

        for (unsigned long i = 0; i < strlen(command); ++i)
        {
            if (command[i] == 'u') user = true;
            else if (command[i] == 'g') group = true;
            else if (command[i] == 'o') other = true;
            else if (command[i] == 'r') read = true;
            else if (command[i] == 'w') write = true;
            else if (command[i] == 'x') execute = true;
            else if (command[i] == '+') add = true;
            else if (command[i] == '-') remove = true;
            else exit(1);
        }
        
        if (stat(filePath, &stats) < 0) 
        {
            exit(1);
        }

        mode_t mode = stats.st_mode;

        if (add) 
        {
            if (user) 
            {
                if (read) mode |= S_IRUSR;
                if (write) mode |= S_IWUSR;
                if (execute) mode |= S_IXUSR;
            }
            if (group) 
            {
                if (read) mode |= S_IRGRP;
                if (write) mode |= S_IWGRP;
                if (execute) mode |= S_IXGRP;
            }
            if (other) 
            {
                if (read) mode |= S_IROTH;
                if (write) mode |= S_IWOTH;
                if (execute) mode |= S_IXOTH;
            }
        }
        else 
        {
            if (user) 
            {
                if (read) mode -= mode & S_IRUSR;
                if (write) mode -= mode & S_IWUSR;
                if (execute) mode -= mode & S_IXUSR;
            }
            if (group) 
            {
                if (read) mode -= mode & S_IRGRP;
                if (write) mode -= mode & S_IWGRP;
                if (execute) mode -= mode & S_IXGRP;
            }
            if (other) 
            {
                if (read) mode -= mode & S_IROTH;
                if (write) mode -= mode & S_IWOTH;
                if (execute) mode -= mode & S_IXOTH;
            }
        }


        if (chmod(filePath, mode) < 0) 
        {
            exit(1);
        }
    }
}

int main(int argc, char** argv)
{   
    char* command;
    char* filePath;

    if (argc != 3)
    {
        exit(1);
    }

    command = argv[1];
    filePath = argv[2];

    changeFileMode(filePath, command);

    return 0;
}
