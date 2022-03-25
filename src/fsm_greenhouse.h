#ifdef _FSM_GREENHOUSE_H_
#define _FSM_GREENHOUSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <poll.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>

#include "fsm.h"
#include "tmr.h"


#define CLK_MS 10// PERIODO ACTUALIZACION MAQUINA DE ESTADOS

//FLAGS DEL SISTEMA



//Funciones


//FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
int comprueba_timer (fsm_t* this);
int comprueba_datos_leidos (fsm_t* this);
int comprueba_datos_escritos (fsm_t* this);
int comprueba_todos_revisados (fsm_t* this);

//FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
void leer(fsm_t* this);
void escribir(fsm_t* this);
void resetear(fsm_t* this);

//SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES

#endif // DEBUG