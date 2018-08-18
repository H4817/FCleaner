#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int isFile(const char *path)
{
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISREG(pathStat.st_mode);
}

int isDirectory(const char *path)
{
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISDIR(pathStat.st_mode);
}

bool isPathCorrect(char *path) {
    return (path && access(path, F_OK | R_OK | W_OK) != -1);
}

int clean(char *path) {
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s '/path/to/your/file'\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (isPathCorrect(argv[1])) {
        return clean(argv[1]);
    } else {
        fprintf(stderr, "'%s' not found or not writable", argv[1]);
        exit(EXIT_FAILURE);
    }
}
