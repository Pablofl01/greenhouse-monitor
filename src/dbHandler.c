#include "dbHandler.h"

char *error;
char *tempDeviceName;
extern char **checked_macs;
extern int checked_devices;
extern sqlite3 *db;

int createTables()
{
    char *sql;
    int dbStatus;

    sql = "CREATE TABLE IF NOT EXISTS devices ("
          "'id' INTEGER PRIMARY KEY AUTOINCREMENT, "
          "'address' VARCHAR(17) NOT NULL UNIQUE, "
          "'name' VARCHAR(17) NOT NULL UNIQUE, "
          "'lastStatus' BOOLEAN DEFAULT 1 NOT NULL) ";
    dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);
    if (dbStatus != SQLITE_OK)
    {
        printf("Error creando la tabla 'devices': %s.\n", error);
        return -2;
    }
    else
        printf("Tabla 'devices' creada.\n");

    sql = "CREATE TABLE IF NOT EXISTS configuration ("
          "'option' VARCHAR(255) PRIMARY KEY, "
          "'value' VARCHAR(255),"
          "'prevValue' VARCHAR(255))";
    dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);
    if (dbStatus != SQLITE_OK)
    {
        printf("Error creando la tabla 'configuration': %s.\n", error);
        return -3;
    }
    else
        printf("Tabla 'configuration' creada.\n");

    return 0;
}

int writeDevices(char **checked_macs)
{
    char sql[88];
    int dbStatus;

    char *drop = "DELETE FROM devices";

    dbStatus = sqlite3_exec(db, drop, NULL, 0, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error al eliminar datos antiguos de la tabla 'devices': %s.\n", error);
        return -1;
    }
    
    for (int i = 0; i < checked_devices; i++)
    {
        char deviceMAC[MAX_MAC_LENGTH];
        strncpy(deviceMAC, checked_macs[i], MAX_MAC_LENGTH);
        int len = strlen(deviceMAC);

        for (int j = 0; j < len; j++)
        {
            if (deviceMAC[j] == ':')
            {
                for (int k = j; k < len; k++)
                {
                    deviceMAC[k] = deviceMAC[k + 1];
                }
                len--;
                j--;
            }
        }
        char deviceName[12];
        strncpy(deviceName, deviceMAC, 12);

        snprintf(sql, 88, "INSERT INTO devices (id, address, name) VALUES(%d, '%s', '%s');", i, checked_macs[i], deviceName);
        dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);
        if (dbStatus != SQLITE_OK)
        {
            printf("Error insertando datos en la tabla 'devices': %s.\n", error);
            return -2;
        }

        char createTable[1024];
        snprintf(createTable, 1024, "CREATE TABLE IF NOT EXISTS '%s' ('value' INTEGER, 'timestamp' INTEGER);", deviceName);
        dbStatus = sqlite3_exec(db, createTable, NULL, 0, &error);
        if (dbStatus != SQLITE_OK)
        {
            printf("Error creando la tabla del dispositivo %s: %s.\n", deviceName, error);
            return -3;
        }
    }

    printf("Tabla 'devices' modificada.\n");

    return 0;
}

int writeConfiguration()
{
    char options[][255] = {
        "waitTime"};

    int values[7] = {
        60};

    for (int i = 0; i < sizeof(values) / sizeof(values[0]); i++)
    {
        if (writeConfig(options[i], values[i]) != 0)
        {
            printf("Error al escribir la base de datos.\n");
            return -5;
        }
    }
    return 0;
}

int initializeDB(char *dbName)
{
    int dbStatus;

    dbStatus = sqlite3_open(dbName, &db);
    if (dbStatus)
    {
        printf("Error al acceder a la base de datos. Revisa los permisos del directorio.\nError: %s.\n", sqlite3_errmsg(db));
        return -1;
    }
    else
        printf("Base de datos inizializada correctamente.\n");

    if (createTables() != 0)
    {
        printf("Error al crear las tablas de base de datos. Es probable que existan tablas con el mismo nombre.\nError: %s.\n", sqlite3_errmsg(db));
        return -2;
    }
    else
        printf("Tablas creadas correctamente.\n");

    if (writeConfiguration() != 0)
    {
        printf("Error al introducir las direcciones MAC en la base de datos.\nError: %s.\n", sqlite3_errmsg(db));
        return -3;
    }
    else
        printf("Tabla de configuración creada correctamente.\n");

    if (writeDevices(checked_macs) != 0)
    {
        printf("Error al introducir los datos en la base de datos.\nError: %s.\n", sqlite3_errmsg(db));
        return -4;
    }
    else

        printf("Tabla de dispositivos creada correctamente.\n");

    return 0;
}

static int selectPrint(void *nothing, int argc, char **argv, char **colNames)
{
    int i;

    for (i = 0; i < argc; i++)
    {
        printf("%s => %s\n", colNames[i], argv[i]);
    }
    printf("\n");
    return 0;
}

int writeConfig(char *option, int value)
{
    int dbStatus;
    char sql[1024];

    snprintf(sql, 1024, "INSERT OR REPLACE INTO configuration(option, value, prevValue) VALUES('%s', %d, %d);", option, value, value);

    dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error al modificar la configuración: %s.\n", error);
        sqlite3_free(error);
        return -1;
    }

    return 0;
}

int getDevice(int id, char *deviceMAC)
{
    int dbStatus;
    char sql[1024];

    snprintf(sql, 1024, "SELECT name FROM devices WHERE id=%d;", id);

    dbStatus = sqlite3_exec(db, sql, retrieveName, deviceMAC, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error al recuperar el sensor %d: %s.\n", id, error);
        sqlite3_free(error);
        return -1;
    }

    return 0;
}

static int retrieveName(char *deviceMAC, int argc, char **argv, char **colNames)
{
    strncpy(deviceMAC, argv[0], 12);
    return 0;
}

int writeData(char *device, int value)
{
    int dbStatus;
    char sql[1024];

    snprintf(sql, 1024, "INSERT INTO '%s' (timestamp, value) VALUES (%d, %d);", device, time(NULL), value);

    dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error al insertar los datos del sensor %s: %s.\n", device, error);
        sqlite3_free(error);
        return -1;
    }

    return 0;
}

int sensorDown(int deviceId)
{
    int dbStatus;
    char sql[1024];

    snprintf(sql, 1024, "UPDATE devices SET 'lastStatus'=0 WHERE id=%d;", deviceId);

    dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);

    if (dbStatus != SQLITE_OK)
    {
        sqlite3_free(error);
        return -1;
    }

    return 0;
}

int checkSensor(int deviceId)
{
    int dbStatus;
    char sql[1024];

    snprintf(sql, 1024, "SELECT lastStatus FROM devices WHERE id=%d;", deviceId);

    int status = 1;

    dbStatus = sqlite3_exec(db, sql, readStatus, &status, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error al deshabilitar el sensor %s: %s.\n", deviceId, error);
        sqlite3_free(error);
        return -1;
    }

    return status;
}

static int readStatus(int *status, int argc, char **argv, char **colNames)
{
    if (argv[0] == 0) status = 0;
    return 0;
}

int compareConfig()
{
    int dbStatus;
    char sql[1024];

    snprintf(sql, 1024, "SELECT value, prevValue FROM configuration WHERE option='%s';", "waitTime");

    int status = 0;

    dbStatus = sqlite3_exec(db, sql, compareValues, &status, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error al comprobar las configuraciones: %s.\n", error);
        sqlite3_free(error);
        return -1;
    }

    return status;
}

static int compareValues(int *status, int argc, char **argv, char **colNames)
{
    status = 1;
    if (argv[0] == argv[1])
    {
        status = 0;
        int dbStatus;
        char sql[1024];

        snprintf(sql, 1024, "UPDATE configuration SET prevValue=%d;", argv[0]);

        dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);

        if (dbStatus != SQLITE_OK)
        {
            printf("Error al actualizar las configuraciones: %s.\n", error);
            sqlite3_free(error);
            return -1;
        }
    }
    return 0;
}

int readTime()
{
    int dbStatus;
    char sql[1024];

    snprintf(sql, 1024, "SELECT value FROM configuration WHERE option='waitTime';");

    int time = 0;

    dbStatus = sqlite3_exec(db, sql, readStatus, &time, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error al leer la configuración: %s.\n", error);
        sqlite3_free(error);
        return -1;
    }

    return time;
}

static int resolveTime(int *time, int argc, char **argv, char **colNames)
{
    strncpy(time, argv[0], 1);
    return 0;
}