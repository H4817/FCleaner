#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

bool isPathCorrect(char *path) {
    return (path && access(path, F_OK|R_OK|W_OK) != -1);
}

int main(int argc, char **argv) {
    if (argc == 2 && isPathCorrect(argv[1])) {
        return 1;
    }
    return 0;
}
