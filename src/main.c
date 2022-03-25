#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <wiringPi.h>
#include "fsm_greenhouse.h"
#include "fsm.h"
#include "tmr.h"

#define TIEMPO_ESPERA 5000 //TIEMPO DE ESPERA ENTRE MEDIDAS (ms)

int FLAG_TIMER =0;

//DEFINICION DE LOS ESTADOS

enum fsm_state{
        SCAN_READ_DATA =0,
        WRITE_DATA =1,
        WAIT = 2,
};

void timer_isr (union sigval value){
	FLAG_TIMER = 1;
}
#include "files.h"
#include "bt.h"

// Some needed constants.
#define MAX_LINE_LENGTH 256
#define MAX_MAC_LENGTH 17
#define WRITE_VALUE "0x001"
const char *file = "devices.txt";

char **macs;
char **checked_macs;
char *data, *result;

int aux() {

	int device_number = count_lines(file);

	if (device_number == -1) {
		printf("Error recuperando el número de dispositivos.\n");
		return -1;
	}

	printf("Número de dispositivos listados: %d\n", device_number);

	macs = malloc(device_number);
	for (int i=0; i<device_number; i++) {
		macs[i] = malloc(MAX_MAC_LENGTH);
	}

	if (read_lines(file, macs) == -1) {
		printf("Error recuperando las direcciones de los dispositivos.\n");
		return -2;
	}

	/**for (int i=0; i<device_number; i++) {
		printf("%s\n", macs[i]);
	}**/

	checked_macs = (char**) malloc(device_number);
	for (int i=0; i<device_number; i++) {
		checked_macs[i] = (char*) malloc(MAX_MAC_LENGTH);
	}

	if (check_macs(device_number, macs, checked_macs) == 0) {
		for (int i=0; i<device_number; i++) {
			if (strcmp(checked_macs[i], "") != 0) write_lines("output.txt", checked_macs[i]);
		}
	}

	data = malloc(MAX_LINE_LENGTH);
	result = malloc(MAX_LINE_LENGTH);

	for (int i=0; i<device_number; i++) {
		if (strcmp(checked_macs[i], "") != 0) {
			write_char(checked_macs[i], WRITE_VALUE, result);
			read_char(checked_macs[i], data);
			write_lines("output.txt", (char*) data);
		}
	}

	free(data);
	free(macs);
	free(checked_macs);

	return 0;
}


//ESPERA HASTA LA PROXIMA ACTIVACION DEL RELOJ


// wait until next_activation (absolute time)

//FUNCIONES DE TRANSICION DE LA MAQUINA DE ESTADOS 
    //Return 0 si no cumplen la transicion
int comprueba_timer (fsm_t* this){
    return 1;
}
int comprueba_datos_leidos (fsm_t* this){
    return 1;
}
int comprueba_datos_escritos (fsm_t* this){
    return 0;
}
int comprueba_todos_revisados (fsm_t* this){
    return 1;
}

//FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
void leer(fsm_t* this){
	printf("SCAN_READ_DATA\n");
	
}
void escribir(fsm_t* this){
    printf("WRITE_DATA\n");
}
void resetear(fsm_t* this){
    printf("WAIT\n");
	FLAG_TIMER=0;
	if (FLAG_TIMER==0)
	{
		tmr_t* tmr = tmr_new(timer_isr);
		tmr_startms(tmr,TIEMPO_ESPERA);
	}
	while(FLAG_TIMER==0){

	}
}




int main()
{
	printf("Comienzo programa\n");

	fsm_trans_t greenhouse_tabla[]={
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
	}
	
	return (0);
}
