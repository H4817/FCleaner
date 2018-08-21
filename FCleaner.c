#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ftw.h>

#ifndef USE_FDS
#define USE_FDS 15
#endif

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
        printf("The file: '%s' deleted successfully\n", path);
    } else {
        printf("Unable to delete: '%s'\n", path);
    }
    return 0;
}

int removeItems(const char *filepath, const struct stat *info,
                const int typeflag, struct FTW *pathinfo) {
    if (typeflag == FTW_SL || typeflag == FTW_SLN)
        remove(filepath);
    else if (typeflag == FTW_F)
        removeFile(filepath);
    else if (typeflag == FTW_DP || typeflag == FTW_D) {
        rmdir(filepath) ?
        printf("Cannot delete the directory: '%s'", filepath) :
        printf("The directory: '%s' deleted successfully\n", filepath);
    }
    else if (typeflag == FTW_DNR) {
        printf(" %s/ (unreadable)\n", filepath);
        rmdir(filepath);
    }

    return 0;
}


int removeDirectory(const char *const dirpath) {
    int result;
    if (dirpath == NULL || *dirpath == '\0')
        return errno = EINVAL;

    result = nftw(dirpath, removeItems, USE_FDS, FTW_DEPTH | FTW_PHYS);
    if (result >= 0)
        errno = result;

    return errno;
}

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

bool isPathCorrect(const char *path) {
    return (path && *path != '\0' && access(path, F_OK | R_OK | W_OK) != -1);
}

int clean(const char *path) {
    if (isFile(path)) {
        return removeFile(path);
    } else if (isDirectory(path)) {
        if (removeDirectory(path)) {
            fprintf(stderr, "%s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
        return 0;
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
        fprintf(stderr, "'%s' not found or not writable\n", argv[1]);
        exit(EXIT_FAILURE);
    }
}
