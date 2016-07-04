#include "task.h"
#include "busy_wait.h"

#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>

/* Lunghezza dell'iperperiodo */
#define H_PERIOD_ 20

/* Numero di frame */
#define NUM_FRAMES_ 5

/* Numero di task */
#define NUM_P_TASKS_ 5

void task0_code();
void task1_code();
void task21_code();
void task22_code();
void task23_code();
void ap_task_code();

/* Cicli di un task prima di richiamare AP TASK */
int ap_exec_index;

/**********************/

/* Questo inizializza i dati globali */
const unsigned int H_PERIOD = H_PERIOD_;
const unsigned int NUM_FRAMES = NUM_FRAMES_;
const unsigned int NUM_P_TASKS = NUM_P_TASKS_;

task_routine P_TASKS[NUM_P_TASKS_];
task_routine AP_TASK;
int * SCHEDULE[NUM_FRAMES_];

void task_init()
{
	/* Inizializzazione di P_TASKS[] */
	P_TASKS[0] = task0_code;
	P_TASKS[1] = task1_code;
	P_TASKS[2] = task21_code;
	P_TASKS[3] = task22_code;
	P_TASKS[4] = task23_code;

	/* Inizializzazione di AP_TASK */
	AP_TASK = ap_task_code;

	/* Inizializzazione di SCHEDULE */

	/* frame 0 */
	SCHEDULE[0] = (int *) malloc( sizeof( int ) * 4 );
	SCHEDULE[0][0] = 0;
	SCHEDULE[0][1] = 1;
	SCHEDULE[0][2] = 2;
	SCHEDULE[0][3] = -1;

	/* frame 1 */
	SCHEDULE[1] = (int *) malloc( sizeof( int ) * 3 );
	SCHEDULE[1][0] = 0;
	SCHEDULE[1][1] = 3;
	SCHEDULE[1][2] = -1;

	/* frame 2 */
	SCHEDULE[2] = (int *) malloc( sizeof( int ) * 3 );
	SCHEDULE[2][0] = 0;
	SCHEDULE[2][1] = 1;
	SCHEDULE[2][2] = -1;	

	/* frame 3 */
	SCHEDULE[3] = (int *) malloc( sizeof( int ) * 3 );
	SCHEDULE[3][0] = 0;
	SCHEDULE[3][1] = 1;
	SCHEDULE[3][2] = -1;

	/* frame 4 */
	SCHEDULE[4] = (int *) malloc( sizeof( int ) * 4 );
	SCHEDULE[4][0] = 0;
	SCHEDULE[4][1] = 1;
	SCHEDULE[4][2] = 4;
	SCHEDULE[4][3] = -1;

	/* Custom Code */
	busy_wait_init();
	ap_exec_index = 0;
}

void task_destroy()
{
	unsigned int i;

	/* Custom Code */

	for ( i = 0; i < NUM_FRAMES; ++i )
		free( SCHEDULE[i] );
}

/**********************************************************/

/* Nota: nel codice dei task e' lecito chiamare ap_task_request() */

void task0_code()
{
	busy_wait(8);

	// Ogni 6 esecuzioni ho un ritardo molto grande
	if ( ap_exec_index == 0 ){
		busy_wait(35);
	}
	ap_exec_index ++;
	ap_exec_index = ap_exec_index % 6;

}

void task1_code()
{
	busy_wait(18);
}

void task21_code()
{
	busy_wait(8);
}

void task22_code()
{
	busy_wait(27);
}

void task23_code()
{
	busy_wait(8);
}

void ap_task_code()
{
	busy_wait(10);
}
