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
#include <time.h>

#ifndef USE_FDS
#define USE_FDS 15
#endif

#if defined _WIN32 || defined __WIN32__ || defined __CYGWIN__ || defined __EMX__ || defined __DJGPP__
/* Win32, Cygwin, OS/2, DOS */
# define ISSLASH(C) ((C) == '/' || (C) == '\\')
#endif

#ifndef ISSLASH
# define ISSLASH(C) ((C) == '/')
#endif

# ifndef FILE_SYSTEM_PREFIX_LEN
#  if FILE_SYSTEM_ACCEPTS_DRIVE_LETTER_PREFIX
/* This internal macro assumes ASCII, but all hosts that support drive
       letters use ASCII.  */
#   define _IS_DRIVE_LETTER(c) (((unsigned int) (c) | ('a' - 'A')) - 'a' \
				<= 'z' - 'a')
#   define FILE_SYSTEM_PREFIX_LEN(Filename) \
	   (_IS_DRIVE_LETTER ((Filename)[0]) && (Filename)[1] == ':' ? 2 : 0)
#  else
#   define FILE_SYSTEM_PREFIX_LEN(Filename) 0
#  endif
# endif



static char const nameset[] =
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_.";

enum { N_ITERATIONS = 8 };

unsigned char *generateRandomBytes(size_t numBytes)
{
    unsigned char *b = malloc (numBytes);
    size_t i;

    for (i = 0; i < numBytes; i++)
    {
        b[i] = rand();
    }

    return b;
}

char * getName (char const *filepath)
{
    char const *base = filepath + FILE_SYSTEM_PREFIX_LEN (filepath);
    char const *p;
    bool sawSlash = false;

    while (ISSLASH (*base))
        base++;

    for (p = base; *p; p++)
    {
        if (ISSLASH (*p))
            sawSlash = true;
        else if (sawSlash)
        {
            base = p;
            sawSlash = false;
        }
    }

    return (char *) base;
}

int removeFile(const char *path) {
    for (int i = 0; i < N_ITERATIONS; ++i) {
        FILE *pFile;
        long lSize;
        unsigned char *buffer;

        pFile = fopen(path, "rb+");
        if (pFile == NULL) {
            fputs("File error", stderr);
            return -1;
        }

        fseek(pFile, 0, SEEK_END);
        lSize = ftell(pFile);
        rewind(pFile);

        buffer = generateRandomBytes(sizeof(unsigned char) * lSize);
        if (buffer == NULL) {
            fputs("Memory error", stderr);
            return -2;
        }

        if (lSize != 0 && fwrite(buffer, (sizeof(unsigned char) * lSize), 1, pFile) != 1) {
            fprintf(stderr, "'%s' cleaning memory error\n", path);
            return -3;
        }

        sync();
        fclose(pFile);
        free(buffer);
    }
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
        printf("Cannot delete the directory: '%s'\n", filepath) :
        printf("The directory: '%s' deleted successfully\n", filepath);
    }
    else if (typeflag == FTW_DNR) {
        printf(" %s/ (unreadable)\n", filepath);
        rmdir(filepath);
    }

    return 0;
}


int removeDirectory(const char *const dirpath) {
    return nftw(dirpath, removeItems, USE_FDS, FTW_DEPTH | FTW_PHYS);
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
        return removeDirectory(path);
    }
    return -1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s '/path/to/your/file'\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (isPathCorrect(argv[1])) {
        srand ((unsigned int) time (NULL));
        return clean(argv[1]);
    } else {
        fprintf(stderr, "'%s' not found or not writable\n", argv[1]);
        exit(EXIT_FAILURE);
    }
}
