#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <wiringPi.h>

#include "constants.h"
#include "fsm_greenhouse.h"
#include "fsm.h"
#include "tmr.h"
#include "files.h"
#include "bt.h"
#include "dbHandler.h"

// Some needed constants
char *data, *result;
int device_id = 0;
char **checked_macs;
int checked_devices;
int valid_data = 1;
const char *file = "devices.txt";
const char *dbName = "test.sqlite";

const sqlite3 *db;
int FLAG_TIMER = 0;

//DEFINICION DE LOS ESTADOS

enum fsm_state
{
	SCAN_READ_DATA = 0,
	WRITE_DATA = 1,
	WAIT = 2,
};

void timer_isr(union sigval value)
{
	FLAG_TIMER = 1;
}

//ESPERA HASTA LA PROXIMA ACTIVACION DEL RELOJ

// wait until next_activation (absolute time)

//FUNCIONES DE TRANSICION DE LA MAQUINA DE ESTADOS
//Return 0 si no cumplen la transicion
int comprueba_timer(fsm_t *this)
{
	return 1;
}
int comprueba_datos_leidos(fsm_t *this)
{
	return 1;
}
int comprueba_datos_escritos(fsm_t *this)
{
	if (device_id < checked_devices)
		return 1;
	return 0;
}
int comprueba_todos_revisados(fsm_t *this)
{
	if (device_id < checked_devices - 1)
	{
		device_id++;
		return 0;
	}
	return 1;
}

//FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
void leer(fsm_t *this)
{
	data = malloc(MAX_LINE_LENGTH);
	result = malloc(MAX_LINE_LENGTH);

	write_char(checked_macs[device_id], WRITE_VALUE, result);
	valid_data = read_char(checked_macs[device_id], data);
	return;
}

void escribir(fsm_t *this)
{
	if (valid_data == 0)
	{

		char to_print[4];
		to_print[0] = data[27];
		to_print[1] = data[28];
		to_print[2] = data[24];
		to_print[3] = data[25];

		int num = (int)strtol(to_print, NULL, 16);

		writeValues("output.txt", num, device_id);
	}
	else if (valid_data == -1)
		printf("No se han recogido correctamente los datos del sensor %d\n", device_id);

	return;
}

void resetear(fsm_t *this)
{
	device_id = 0;
	printf("WAIT\n");
	FLAG_TIMER = 0;
	if (FLAG_TIMER == 0)
	{
		tmr_t *tmr = tmr_new(timer_isr);
		tmr_startms(tmr, TIEMPO_ESPERA);
	}
	while (FLAG_TIMER == 0)
	{
	}
}

int initialize()
{
	if (initializeFiles(file) != 0)
	{
		return -1;
	}
	return initializeDB(dbName);
}

int main()
{
	printf("Comienzo programa\n");
	if (initialize() != 0)
		return -1;


	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < checked_devices; j++)
		{
			char *deviceName = (char*) malloc(12);
			if (getDevice(j, deviceName) != 0)
				return -2;
			writeData(deviceName, ((rand() % 10-1+1)+1));
			delay(30000);
		}
	}

	/**fsm_trans_t greenhouse_tabla[]={
			{WAIT,comprueba_timer,SCAN_READ_DATA,leer},
			{SCAN_READ_DATA,comprueba_datos_leidos,WRITE_DATA,escribir},
			{WRITE_DATA,comprueba_todos_revisados,WAIT,resetear},
			{WRITE_DATA,comprueba_datos_escritos,SCAN_READ_DATA,leer},
			{-1,NULL,-1,NULL},
	};
	fsm_t* greenhouse_fsm = fsm_new(SCAN_READ_DATA,greenhouse_tabla,NULL);

	

	while (1)
	{
		fsm_fire(greenhouse_fsm);
	}**/

	free(data);
	//free(checked_macs);

	return 0;
}
