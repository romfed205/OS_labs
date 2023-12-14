#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>
#include <grp.h>
#include <time.h>

int total = 0;
struct stat gst;

void printWithColor(char* fileName)
{
    printf("\033[");

    mode_t m = gst.st_mode & S_IFMT;

    switch (gst.st_mode & S_IFMT)
    {
        case S_IFLNK: printf("96"); break;
        case S_IFDIR: printf("94"); break;
        default:
        {
            if (gst.st_mode & S_IXOTH)
            {
                printf("92");
            }
            else
            {
                printf("39");
            }
        }
    }
    printf("m%s", fileName);
    printf("\033[0m");
    printf("\033[0m");
}

void printFileInfo(char* filePath)
{
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if (lstat(filePath, &st) == -1)
    {
        printf("error\n");
        exit(1);
    }

    total += (st.st_blocks / 2);

    switch (st.st_mode & S_IFMT)
    {
        case S_IFLNK:   printf("l");   break;
        case S_IFSOCK:  printf("s");   break;
        case S_IFDIR:   printf("d");   break;
        default:        printf("-");   break;
    }

    printf((st.st_mode & S_IRUSR) ? "r" : "-");
    printf((st.st_mode & S_IWUSR) ? "w" : "-");
    printf((st.st_mode & S_IXUSR) ? "x" : "-");
    printf((st.st_mode & S_IRGRP) ? "r" : "-");
    printf((st.st_mode & S_IWGRP) ? "w" : "-");
    printf((st.st_mode & S_IXGRP) ? "x" : "-");
    printf((st.st_mode & S_IROTH) ? "r" : "-");
    printf((st.st_mode & S_IWOTH) ? "w" : "-");
    printf((st.st_mode & S_IXOTH) ? "x" : "-");

    printf(" %4.lu", st.st_nlink);

    struct passwd* user = getpwuid(st.st_uid);
    printf(" %s", (user != 0) ? user->pw_name : " ");

    struct group* group = getgrgid(st.st_gid);
    printf(" %s", (group != 0) ? group->gr_name : " ");

    printf(" %7.ld", st.st_size);

    struct tm* time_ptr;
    time_t time = st.st_ctime;
    time_ptr = localtime(&time);

    char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    printf(" %s", months[time_ptr->tm_mon]);
    printf(" %02d", time_ptr->tm_mday);
    printf(" %02d:%02d ", time_ptr->tm_hour, time_ptr->tm_min);
}

bool isDir(struct stat* st)
{
    return st->st_mode & S_IFDIR;
}

bool isHiddenFile(char* filePath)
{
    if (filePath[0] == '.')
    {
        return true;
    }

    return false;
}

void listDir(char* dirPath, bool printDirName, bool aFlag, bool lFlag)
{
    DIR* dir = opendir(dirPath);
    struct dirent* file;

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    stat(dirPath, &st);

    if (isDir(&st))
    {
        if (printDirName)
        {
            printf("%s:\n", strrchr(dirPath, '/'));
        }

        while ((file = readdir(dir))!= NULL)
        {
            if (!aFlag && isHiddenFile(file->d_name))
            {
                continue;
            }

            char filePath[PATH_MAX] = "";
            strcat(filePath, dirPath);
            strcat(filePath, "/");
            strcat(filePath, file->d_name);

            if (lFlag)
            {
                printFileInfo(filePath);
            }

            memset(&gst, 0, sizeof(struct stat));
            lstat(filePath, &gst);

            printWithColor(file->d_name);
            printf((lFlag)? "\n" : " ");
        }

        if (lFlag)
        {
            printf("total %ld\n", total);
        }

        if (printDirName)
        {
            printf((lFlag)? "\n" : "\n\n");
        }
    }
    else
    {
        if (lFlag)
        {
            printFileInfo(dirPath);
            printf("%s", dirPath);
            printf((printDirName)? "\n\n" : "");
        }
        else
        {
            printf("%s ", dirPath);
        }
    }
}

int main(int argc, char** argv)
{
    int opt;
    bool aFlag = false;
    bool lFlag = false;

    while ((opt = getopt(argc, argv, "la")) != -1)
    {
        switch (opt)
        {
            case 'l':
            {
                lFlag = true;
                break;
            }
            case 'a':
            {
                aFlag = true;
                break;
            }
            case '?':
            {
                exit(1);
            }
        }
    }

    if (optind == argc)
    {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        listDir(cwd, false, aFlag, lFlag);
    }

    bool printDirName = (optind + 1 < argc)? true : false;

    while (optind < argc)
    {
        listDir(argv[optind++], printDirName, aFlag, lFlag);
    }

    printf("\n");

    return 0;
}
