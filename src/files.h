/*
 * files.h
 *
 *  Created on: 14 mar. 2022
 *      Author: p.flopez@alumnos.upm.es
 */

#ifndef FILES_H_
#define FILES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <errno.h>

int count_lines (char *filename);
int read_lines (char *filename, char **read);
int write_lines (char *filename, char *write);

#endif // FILES_H_
