#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>

void printFile(char* filePath, bool nFlag, bool bFlag, bool eFlag)
{
    FILE* file = NULL;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t lineNum = 1;

    file = fopen(filePath, "r");

    if (file == NULL)
    {
        printf("error");
        exit(1);
    }

    while ((read = getline(&line, &len, file)) != -1)
    {
        if (nFlag || bFlag)
        {
            printf("     ");
        }

        if (bFlag && (strlen(line) != 1) || !bFlag && nFlag)
        {
            printf("%lu  ", lineNum);
            ++lineNum;
        }

        if (eFlag)
        {
            for (size_t i = 0; i < strlen(line); ++i)
            {
                if (line[i] == '\n')
                {
                    printf("$");
                }

                printf("%c", line[i]);
            }
        }
        else
        {
            printf("%s", line);
        }
    }

    fclose(file);
    free(line);
}

int main(int argc, char** argv)
{
    int opt;
    bool nFlag = false;
    bool bFlag = false;
    bool eFlag = false;

    while ((opt = getopt(argc, argv, "nbE")) != -1)
    {
        switch (opt)
        {
        case 'n':
        {
            nFlag = true;
            break;
        }
        case 'b':
        {
            bFlag = true;
            break;
        }
        case 'E':
        {
            eFlag = true;
            break;
        }
        case '?':
        {
            printf("error");
            exit(1);
        }
        }
    }

    if (optind == argc)
    {
        printf("error");
        exit(1);
    }

    while (optind < argc)
    {
        printFile(argv[optind++], nFlag, bFlag, eFlag);
    }

    return 0;
}