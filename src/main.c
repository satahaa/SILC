#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        printf("No file has been provided\n");
        exit(EXIT_FAILURE);
    }
    char* path = argv[1];
    const size_t len = strlen(path);
    if (path[len - 1] != 'r' || path[len - 2] != 'o' || path[len - 3] != 'c' || path[len - 4] != '.' || len < 4) {
        printf("Invalid filetype: %s, file has to end with .cor extension", path);
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("File not found: %s\n", path);
        exit(EXIT_FAILURE);
    }

    int cur = fgetc(file);
    while (cur != EOF) {
        printf("%c", cur);
        cur = fgetc(file);
    }

    if (argc > 2) {
        printf("%s.exe\n", argv[2]);
    }
    else {
        printf("a.out\n");
    }

    return 0;
}

