#ifndef DBHANDLER_H_
#define DBHANDLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sqlite3.h>
#include "constants.h"

int initializeDB (char *dbName);
int createTables();
int readEntries (char *table);
static int selectPrint(void *nothing, int argc, char **argv, char **colNames);
int writeConfig(char *option, int value);
int writeDevices(char **checked_macs);
int getDevice(int id, char *deviceName);
static int retrieveName(char *deviceName, int argc, char **argv, char **colNames);
int writeData(char *device, int value);

#endif // DBHANDLER_H_