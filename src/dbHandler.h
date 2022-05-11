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
static int selectPrint(void *nothing, int argc, char **argv, char **colNames);
int writeConfig(char *option, int value);
int writeDevices(char **checked_macs);
int getDevice(int id, char *deviceName);
static int retrieveName(char *deviceName, int argc, char **argv, char **colNames);
int writeData(char *device, int value);
int sensorDown(int deviceId);
int checkSensor(int deviceId);
static int readStatus(int *status, int argc, char **argv, char **colNames);
int compareConfig();
static int compareValues(int *status, int argc, char **argv, char **colNames);
int readTime();
static int resolveTime(int *time, int argc, char **argv, char **colNames);

#endif // DBHANDLER_H_