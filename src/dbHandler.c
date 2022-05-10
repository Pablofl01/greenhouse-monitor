#include "dbHandler.h"

char *error;
extern char **checked_macs;
extern int checked_devices;

int createTables(sqlite3 *db)
{
    char *sql;
    int dbStatus;

    sql = "CREATE TABLE IF NOT EXISTS devices ("
          "`id` INTEGER PRIMARY KEY AUTOINCREMENT, "
          "`address` VARCHAR(17) NOT NULL UNIQUE, "
          "`name` VARCHAR(17) NOT NULL UNIQUE, "
          "`lastStatus` BOOLEAN DEFAULT 1 NOT NULL) ";
    dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);
    if (dbStatus != SQLITE_OK)
    {
        printf("Error creando la tabla 'devices': %s.\n", error);
        return -2;
    }
    else
        printf("Tabla 'devices' creada.\n");

    sql = "CREATE TABLE IF NOT EXISTS configuration ("
          "`option` VARCHAR(255) PRIMARY KEY, "
          "`value` VARCHAR(255),"
          "`prevValue` VARCHAR(255))";
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

int writeDevices(sqlite3 *db, char **checked_macs)
{
    char sql[1024];
    int dbStatus;

    char drop[] = "DELETE FROM devices";

    dbStatus = sqlite3_exec(db, drop, NULL, 0, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error al eliminar datos antiguos de la tabla 'devices': %s.\n", error);
        return -1;
    }

    for (int i = 0; i < checked_devices; i++)
    {
        char deviceName[MAX_MAC_LENGTH];
        strncpy(deviceName, checked_macs[i], MAX_MAC_LENGTH);
        int len = strlen(deviceName);

        for(int j = 0; j < len; j++) {
		    if(deviceName[j] == ':') {
			    for(int k = j; k < len; k++) {
				    deviceName[k] = deviceName[k + 1];
			    }
			    len--;
			    j--;	
		    } 
	    }	

        snprintf(sql, 1024, "INSERT INTO devices (address, name) VALUES('%s', '%s');", checked_macs[i], deviceName);
        dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);
        if (dbStatus != SQLITE_OK)
        {
            printf("Error insertando datos en la tabla 'devices': %s.\n", error);
            return -2;
        }

        char createTable[1024];
        snprintf(createTable, 1024, "CREATE TABLE IF NOT EXISTS `%s` (`value` INTEGER, `timestamp` DATETIME DEFAULT current_timestamp);", deviceName);
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

int writeConfiguration(sqlite3 *db)
{
    char options[][255] = {
        "waitTime",
        "highTemp",
        "lowTemp",
        "highRelHum",
        "lowRelHum",
        "highHum",
        "lowHum"};

    int values[7] = {
        60,
        45,
        12,
        87,
        11,
        60,
        40};

    for (int i = 0; i < sizeof(values) / sizeof(values[0]); i++)
    {
        if (writeConfig(db, options[i], values[i]) != 0)
        {
            printf("Error al escribir la base de datos.\n");
            return -5;
        }
    }
    return 0;
}

int initializeDB(sqlite3 *db, char *dbName)
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

    if (createTables(db) != 0)
    {
        printf("Error al crear las tablas de base de datos. Es probable que existan tablas con el mismo nombre.\nError: %s.\n", sqlite3_errmsg(db));
        return -2;
    }
    else
        printf("Tablas creadas correctamente.\n");

    if (writeConfiguration(db) != 0)
    {
        printf("Error al introducir las direcciones MAC en la base de datos.\nError: %s.\n", sqlite3_errmsg(db));
        return -3;
    }
    else
        printf("Tabla de dispositivos creada correctamente.\n");

    if (writeDevices(db, checked_macs) != 0)
    {
        printf("Error al introducir los datos en la base de datos.\nError: %s.\n", sqlite3_errmsg(db));
        return -4;
    }
    else
        printf("Tabla de configuración correctamente.\n");

    if (readEntries(db, "devices") != 0)
    {
        printf("Error al leer la tabla 'devices' de la base de datos.\nError: %s.\n", sqlite3_errmsg(db));
        return -5;
    }
    else
        printf("Valores leídos correctamente.\n");

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

int writeConfig(sqlite3 *db, char *option, int value)
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

int writeData(sqlite3 *db, char *device, int value)
{
    int dbStatus;
    char sql[1024];

    snprintf(sql, 1024, "INSERT INTO `%s` (value) VALUES (%d);", device, value);

    dbStatus = sqlite3_exec(db, sql, NULL, 0, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error al insertar los datos del sensor %s: %s.\n", device, error);
        sqlite3_free(error);
        return -1;
    }

    return 0;
}

int readEntries(sqlite3 *db, char *table)
{
    int dbStatus;
    char sql[1024];

    snprintf(sql, 23, "SELECT * FROM %s;", table);

    dbStatus = sqlite3_exec(db, sql, selectPrint, 0, &error);

    if (dbStatus != SQLITE_OK)
    {
        printf("Error leyendo la tabla '%s': %s.\n", table, sqlite3_errmsg(db));
        sqlite3_free(error);
        return -2;
    }
}