#ifndef FILES_H_
#define FILES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "constants.h"

int countLines (char *filename);
int readLines (char *filename, char **read);
int writeLines (char *filename, char *write);
int initializeFiles(char *file);

#endif // FILES_H_
