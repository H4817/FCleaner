#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int isFile(const char *path) {
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISREG(pathStat.st_mode);
}

int isDirectory(const char *path) {
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISDIR(pathStat.st_mode);
}

int removeFile(const char *path) {
    FILE *pFile;
    long lSize;
    char *buffer;

    pFile = fopen(path, "rb+");
    if (pFile == NULL) {
        fputs("File error", stderr);
        exit(1);
    }

    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);

    buffer = (char *) calloc(lSize, sizeof(char));
    if (buffer == NULL) {
        fputs("Memory error", stderr);
        exit(2);
    }

    fwrite(buffer, (sizeof(char) * lSize), 1, pFile);

    fclose(pFile);
    free(buffer);
    if (remove(path) == 0) {
        printf("The file: '%s' deleted successfully", path);
    } else {
        printf("Unable to delete: '%s'", path);
    }
    return 0;
}

bool isPathCorrect(const char *path) {
    return (path && access(path, F_OK | R_OK | W_OK) != -1);
}

int clean(const char *path) {
    if (isFile(path)) {
        return removeFile(path);
    } else if (isDirectory(path)) {
        //todo
    }
    return -1;
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
