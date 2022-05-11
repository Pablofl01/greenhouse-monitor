#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <wiringPi.h>
#include <pthread.h>

#include "constants.h"
#include "fsm_greenhouse.h"
#include "fsm.h"
#include "tmr.h"
#include "files.h"
#include "bt.h"
#include "dbHandler.h"

int FLAG_TIMER = 0;
int FLAG_DATOS_LEIDOS = 0;
int FLAG_INIT = 0;
int FLAG_CONFIG = 0;

char *data, *result;
int device_id = 0;
char **checked_macs;
int checked_devices;
int valid_data = 1;
const char *file = "devices.txt";
const char *dbName = "greenhouse.sqlite";
const sqlite3 *db;
int TIEMPO_ESPERA = 5000;

//DEFINICION DE LOS ESTADOS

enum fsm_state
{
	SCAN_READ_DATA = 0,
	WRITE_DATA = 1,
	WAIT = 2,
	START = 3,
};

void timer_isr(union sigval value)
{
	FLAG_TIMER = 1;
}

//Codigo de la hebra. Esta hebra comprueba si se ha cambiado la configuracion del sistema
PI_THREAD(myThread0)
{

	if (compareConfig() == 1)
		FLAG_CONFIG = 1;
	// }
}

int ConfiguraInicializaSistema()
{
	if (initializeFiles(file) != 0)
	{
		printf("Error al recuperar los dispositivos del fichero inicial.");
		return -1;
	}

	if (initializeDB(dbName) != 0)
	{
		printf("Error al configurar la base de datos.");
		return -1;
	}

	int waitTime = readTime();

	if (waitTime > 0)
		TIEMPO_ESPERA = 1000 * 3600 * waitTime;

	int x;
	x = piThreadCreate(myThread0);
	if (x != 0)
	{
		printf("it didn't start!!!\n");
	}

	FLAG_INIT = 1;

	return 1;
}

//ESPERA HASTA LA PROXIMA ACTIVACION DEL RELOJ

// wait until next_activation (absolute time)
void delay_until(unsigned int next)
{
	unsigned int now = millis();
	if (next > now)
	{
		delay(next - now);
	}
}

//FUNCIONES DE TRANSICION DE LA MAQUINA DE ESTADOS
//Return 0 si no cumplen la transicion
int comprueba_timer(fsm_t *this)
{
	if (FLAG_TIMER == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
int comprueba_datos_leidos(fsm_t *this)
{
	if (FLAG_DATOS_LEIDOS == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
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
int comprueba_setup_done(fsm_t *this)
{
	//Si las configuraciones iniciales son correctas, la funcion retorna 1
	if (FLAG_INIT == 1)
	{
		return 1;
	}
	return 0;
}
int comprueba_dif_config(fsm_t *this)
{
	if (FLAG_CONFIG == 0)
	{
		return 0;
	}
	else
	{
		FLAG_CONFIG = 0;
		return 1;
	}
}

//FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
void leer(fsm_t *this)
{
	printf("READ\n");
	FLAG_CONFIG = 0;
	FLAG_TIMER = 0;
	data = malloc(MAX_LINE_LENGTH);
	result = malloc(MAX_LINE_LENGTH);

	if (checkSensor(device_id) == 1)
	{
		write_char(checked_macs[device_id], WRITE_VALUE, result);
		valid_data = read_char(checked_macs[device_id], data);

		FLAG_DATOS_LEIDOS = 1;
	}
	return;
}

void escribir(fsm_t *this)
{
	printf("WRITE\n");
	FLAG_DATOS_LEIDOS = 0;
	if (valid_data == 0)
	{

		char to_print[4];
		to_print[0] = data[27];
		to_print[1] = data[28];
		to_print[2] = data[24];
		to_print[3] = data[25];

		int num = (int)strtol(to_print, NULL, 16);

		char *deviceName = (char *)malloc(12);
		if (getDevice(device_id, deviceName) != 0)
			return;
		if (writeData(deviceName, num) != 0)
			return;
		writeValues("output.txt", num, device_id);
	}
	else if (valid_data == -1)
	{
		printf("No se han recogido correctamente los datos del sensor %d\n", device_id);
		if (sensorDown(device_id) != 0)
			return;
	}
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
}

int main()
{
	printf("Comienzo programa\n");
	//int statusInit = ConfiguraInicializaSistema();
	/* 	if (statusInit !=0){
    free(data);
    free(macs);
    free(checked_macs);
    exit(0);
  } */
	unsigned int next = millis();
	fsm_trans_t greenhouse_tabla[] = {
		{START, comprueba_setup_done, SCAN_READ_DATA, leer},
		{WAIT, comprueba_timer, SCAN_READ_DATA, leer},
		{SCAN_READ_DATA, comprueba_datos_leidos, WRITE_DATA, escribir},
		{WRITE_DATA, comprueba_todos_revisados, WAIT, resetear},
		{WRITE_DATA, comprueba_datos_escritos, SCAN_READ_DATA, leer},
		//cONFIG
		{WRITE_DATA, comprueba_dif_config, SCAN_READ_DATA, leer},
		{SCAN_READ_DATA, comprueba_dif_config, SCAN_READ_DATA, leer},
		{WAIT, comprueba_dif_config, SCAN_READ_DATA, leer},
		{-1, NULL, -1, NULL},
	};
	fsm_t *greenhouse_fsm = fsm_new(START, greenhouse_tabla, NULL);

	ConfiguraInicializaSistema();
	while (1) {
		fsm_fire(greenhouse_fsm);
		next += CLK_MS;
		delay_until(next);
	}

	return 0;
}
