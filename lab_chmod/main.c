#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <mode> <file>\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    char *file = argv[2];

    struct stat st;
    if (stat(file, &st) != 0) {
        perror("stat");
        return 1;
    }

    mode_t new_mode = st.st_mode;

    if (mode[0] >= '0' && mode[0] <= '7') {
        new_mode = strtol(mode, NULL, 8);
    } else {
        for (char *p = mode; *p; p++) {
            switch (*p) {
                case '+':
                    p++;
                    switch (*p) {
                        case 'r': new_mode |= S_IRUSR | S_IRGRP | S_IROTH; break;
                        case 'w': new_mode |= S_IWUSR | S_IWGRP | S_IWOTH; break;
                        case 'x': new_mode |= S_IXUSR | S_IXGRP | S_IXOTH; break;
                    }
                    break;
                case '-':
                    p++;
                    switch (*p) {
                        case 'r': new_mode &= ~(S_IRUSR | S_IRGRP | S_IROTH); break;
                        case 'w': new_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH); break;
                        case 'x': new_mode &= ~(S_IXUSR | S_IXGRP | S_IXOTH); break;
                    }
                    break;
            }
        }
    }

    if (chmod(file, new_mode) != 0) {
        perror("chmod");
        return 1;
    }

    return 0;
}