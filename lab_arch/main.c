#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <libgen.h>
#include <sys/mman.h>

#define MAX_NAME_SIZE 30

bool archExists = false;

struct FileData
{
    char fileName[MAX_NAME_SIZE];
    mode_t fileMode;
    off_t fileEnd;
};

int openArch(char* archName, bool createIfNotExist)
{
    char archPath[2048];

    getcwd(archPath, sizeof(archPath));
    strcpy(archPath, "/");
    strcpy(archPath, archName);

    if (access(archPath, F_OK) == 0) 
    {
        archExists = true;
    }

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    int file;
    
    if (createIfNotExist)
    {
        file = open(archPath, O_RDWR | O_CREAT, mode);
    }
    else
    {
        file = open(archPath, O_RDWR, mode);
    }

    if (file == -1)
    {
        printf("Error: can't open or create arch file\n");
        exit(1);
    }

    return file;
}

u_int32_t getFilesCount(int archFile)
{
    u_int32_t filesCount = 0;

    if (archExists)
    {
        if (lseek(archFile, 0, SEEK_SET) == -1)
        {
            printf("Error: can't lseek to file start\n");
            exit(1);
        }

        if (read(archFile, &filesCount, sizeof(filesCount)) != sizeof(filesCount))
        {
            printf("Error: can't read files count\n");
            exit(1);
        }
    }
    else 
    {
        write(archFile, &filesCount, sizeof(filesCount));
    }

    return filesCount;
}

void setFilesCount(int archFile, u_int32_t count)
{
    if (lseek(archFile, 0, SEEK_SET) == -1)
    {
        printf("Error: can't lseek to file start\n");
        exit(1);
    }

    if (write(archFile, &count, sizeof(count)) == -1)
    {
        printf("Error: can't write files count\n");
        exit(1);
    }
}

void printHelp()
{
    printf("./archiver arch_name -i filePath\n");
    printf("./archiver arch_name –e filePath\n");
    printf("./archiver arch_name –s\n");
    printf("./archiver –h\n");
}

void inputFile(char* archName, char* filePath)
{
    char* fileName = basename(filePath);

    if (strlen(fileName) + 1 > MAX_NAME_SIZE)
    {
        printf("Error: file name length must be < %d\n", MAX_NAME_SIZE);
        exit(1);
    }

    struct stat stats;
    if (stat(filePath, &stats) == -1)
    {
        printf("Error: file stat failed\n");
        exit(1);
    }

    int archFile = openArch(archName, true);
    u_int32_t filesCount = getFilesCount(archFile);

    off_t offset = sizeof(filesCount);

    for (u_int32_t i = 0; i < filesCount; ++i)
    {
        if (lseek(archFile, offset, SEEK_SET) == -1)
        {
            printf("Error: lseek failed\n");
            exit(1);
        }

        struct FileData data;

        if (read(archFile, &data, sizeof(struct FileData)) == -1)
        {
            printf("Error: can't read arch\n");
            exit(1);
        }

        if (strcmp(data.fileName, fileName) == 0)
        {
            printf("Error: file is exist in archive\n");
            exit(1);
        }

        offset = data.fileEnd;
    }

    off_t archEnd = lseek(archFile, 0, SEEK_END);

    if (archEnd == -1)
    {
        printf("Error: lseek failed in arhice file\n");
        exit(1);
    }

    struct FileData data;
    strcpy(data.fileName, fileName);
    data.fileMode = stats.st_mode;
    data.fileEnd = archEnd + sizeof(struct FileData) + stats.st_size;

    write(archFile, &data, sizeof(struct FileData));

    int file = open(filePath, O_RDONLY);

    if (file == -1)
    {
        remove(archName);
        printf("Error: can't open file\n");
        exit(1);
    }

    void* fileContent = malloc(stats.st_size);

    int readBytes = 0;
    if ((readBytes = read(file, fileContent, stats.st_size)) == -1)
    {
        remove(archName);
        printf("Error: can't read file\n");
        exit(1);
    }

    write(archFile, fileContent, readBytes);
    setFilesCount(archFile, filesCount + 1);

    close(archFile);
    free(fileContent);
}

void extractFile(char* archName, char* fileName)
{
    int archFile = openArch(archName, false);
    u_int32_t filesCount = getFilesCount(archFile);

    off_t offset = sizeof(filesCount);
    bool flag = false;

    long long deletedFileSize = 0;

    for (u_int32_t i = 0; i < filesCount; ++i)
    {
        long long fileStart;

        if ((fileStart = lseek(archFile, offset, SEEK_SET)) == -1)
        {
            printf("Error: lseek failed\n");
            exit(1);
        }

        struct FileData data;

        if (read(archFile, &data, sizeof(struct FileData)) == -1)
        {
            printf("Error: can't read arch\n");
            exit(1);
        }

        if (flag == true)
        {
            lseek(archFile, offset, SEEK_SET);
            data.fileEnd -= deletedFileSize;
            offset = data.fileEnd;
            write(archFile, &data, sizeof(struct FileData));
            continue;
        }
        else if (strcmp(data.fileName, fileName) != 0)
        {
            offset = data.fileEnd;
            continue;
        }

        flag = true;

        int fileContentStart;
        if ((fileContentStart = lseek(archFile, offset + sizeof(struct FileData), SEEK_SET)) == -1)
        {
            printf("Error: lseek failed\n");
            exit(1);
        }

        long long fileSize = data.fileEnd - fileContentStart;
        void* fileContent = malloc(fileSize);

        if (read(archFile, fileContent, fileSize) == -1)
        {
            printf("Error: can't read arch\n");
            exit(1);
        }

        char filePath[2048];
        strcpy(filePath, fileName);

        if (access(filePath, F_OK) == 0) 
        {
            srand(time(NULL));
            char randomSufix[5];
            sprintf(randomSufix, "%d", 1000+(rand()%9000));

            memcpy(filePath + strlen(fileName), "_", 1);
            memcpy(filePath + strlen(fileName) + 1, randomSufix, strlen(randomSufix) + 1);
        }

        printf("Unzip file %s as %s\n", fileName, filePath);

        int newFile;

        if ((newFile = open(filePath, O_WRONLY | O_CREAT, data.fileMode)) == -1)
        {
            printf("Error: can't open new file\n");
            exit(1);
        }

        if (write(newFile, fileContent, fileSize) == -1)
        {
            printf("Error: can't write to new file\n");
            exit(1);
        }

        long long archLength = lseek(archFile, 0, SEEK_END);

        if (archLength == -1)
        {
            printf("Error: lseek error\n");
            exit(1);
        }

        if (lseek(archFile, 0, SEEK_SET) == -1)
        {
            printf("Error: lseek error\n");
            exit(1);
        }

        void* archStart = mmap(NULL, archLength, PROT_READ | PROT_WRITE, MAP_SHARED, archFile, 0);

        if (archStart == MAP_FAILED)
        {
            printf("Error: mmap error\n");
            exit(1);
        }

        memmove((char*) archStart + fileStart, (char*) archStart + data.fileEnd, archLength - data.fileEnd);
        munmap(archStart, archLength);

        if (ftruncate(archFile, archLength - (data.fileEnd - fileStart)) == -1)
        {
            printf("Error: ftruncate archive file error\n");
            exit(1);
        }

        deletedFileSize = data.fileEnd - fileStart;
        offset = data.fileEnd - deletedFileSize;

        close(newFile);
    }

    if (!flag)
    {
        printf("Error: can't find file with the same name in archive\n");
        exit(1);
    }

    setFilesCount(archFile, getFilesCount(archFile) - 1);
    close(archFile);
}

void statArch(char* archName)
{
    int archFile = openArch(archName, false);
    u_int32_t filesCount = getFilesCount(archFile);
    printf("Files count = %d\n", filesCount);

    off_t offset = sizeof(filesCount);

    for (u_int32_t i = 0; i < filesCount; ++i)
    {
        if (lseek(archFile, offset, SEEK_SET) == -1)
        {
            printf("Error: lseek failed\n");
            exit(1);
        }

        struct FileData data;

        if (read(archFile, &data, sizeof(struct FileData)) == -1)
        {
            printf("Error: can't read arch\n");
            exit(1);
        }

        printf("file_%d = %s    size = %d\n", i + 1, data.fileName, data.fileEnd - (offset + sizeof(data)));
        offset = data.fileEnd;
    }

    close(archFile);
}

int main(int argc, char** argv)
{   
    int opt;
    bool iFlag = false;
    bool eFlag = false;
    bool sFlag = false;
    bool hFlag = false;

    while ((opt = getopt(argc, argv, "iesh")) != -1)
    {
        switch (opt)
        {
            case 'i':
            {
                iFlag = true;
                break;
            }
            case 'e':
            {
                eFlag = true;
                break;
            }
            case 's':
            {
                sFlag = true;
                break;
            }
            case 'h':
            {
                hFlag = true;
                break;
            }
            case '?':
            {
                printf("Error: wrong argv - %c\n", opt);
                exit(1);
            }
        }
    }

    if (hFlag)
    {
        printHelp();
        return 0;
    }
    
    if (sFlag && optind == argc - 1)
    {
        statArch(argv[optind]);
        return 0;
    }

    if (argc < 4)
    {
        printf("Error: wrong argc\n");
        exit(1);
    }

    char* archName = argv[optind++];

    if (iFlag)
    {        
        for (int i = optind; i < argc; ++i)
        {
            inputFile(archName, argv[i]);            
        }
    }
    else if (eFlag)
    {
        for (int i = optind; i < argc; ++i)
        {
            extractFile(archName, argv[i]);      
        }
    }

    return 0;
}