#include "macros.h"


FILE *open_file(char *filename, char *mode) {
    if (filename == NULL) {
        printf("Filename is NULL\n");
        exit(1);
    }

    FILE *fp = fopen(filename, mode);
    if (fp == NULL) {
        printf("Could not open file %s\n", filename);
        exit(1);
    }

    return fp;
}

void close_file(FILE *fp) {
    if (fp == NULL) {
        printf("File pointer is NULL\n");
        exit(1);
    }

    fclose(fp);
    return;
}