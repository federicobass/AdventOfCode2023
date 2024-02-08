#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


FILE *open_file(char *filename, char *mode);
void close_file(FILE *fp);