#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>

void grepFile(char* filePath, char* pattern, bool pipeFlag)
{
    FILE* file = NULL;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t lineNum = 1;

    if (pipeFlag)
    {
        file = stdin;
    }
    else
    {
        file = fopen(filePath, "r");

        if (file == NULL)
        {
            printf("error");
            exit(1);
        }
    }

    while ((read = getline(&line, &len, file)) != -1)
    {
        if (strstr(line, pattern))
        {
            printf("%s", line);
        }
    }

    if (!pipeFlag)
    {
        fclose(file);
    }

    free(line);
}

int main(int argc, char** argv)
{
    int opt;
    bool nFlag = false;
    bool bFlag = false;
    bool eFlag = false;

    if (argc == 1)
    {
        printf("error");
        exit(1);
    }

    char* pattern = argv[1];
    bool pipeFlag = (argc == 2)? true : false;

    if (pipeFlag)
    {
        grepFile(NULL, pattern, pipeFlag);
    }
    else
    {
        for (int i = 2; i < argc; ++i)
        {
            grepFile(argv[i], pattern, pipeFlag);
        }
    }

    return 0;
}