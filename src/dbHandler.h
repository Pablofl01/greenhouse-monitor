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

int initializeDB (sqlite3 *db, char *dbName);
int createTables(sqlite3 *db);
int readEntries (sqlite3 *db, char *table);
static int selectPrint(void *nothing, int argc, char **argv, char **colNames);
int writeConfig(sqlite3 *db, char *option, int value);
int writeDevices(sqlite3 *db, char **checked_macs);
//int write_entries (char *table, char *write);

#endif // DBHANDLER_H_