#include <stdio.h>
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