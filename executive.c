/* traccia dell'executive (pseudocodice) */

#ifdef MULTIPROC
#define _GNU_SOURCE
#endif

#define AP_TASK_INDEX 0		///< Utilizzato per compatibilità della struttura dati quando si utilizza un monitor per un singolo task
#define TIME_QUANT 10		///< Quanto temporale da 10 ms

#define AP_FLAG_ON 1				
#define AP_FLAG_OFF 0

#define AP_TASK_PRIO 1
#define EXECUTIVE_PRIO sched_get_priority_max(SCHED_FIFO)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "task.h"
#include "monitor.h"
#include "executive.h"

pthread_mutex_t exe_mtx;	///< Mutex riservato all'executive

int ap_flag_request;

#ifdef TIMELOG
struct timeval start;
#endif

task_monitor* monitor;
task_monitor* ap_monitor;

void ap_task_request()
{
	// Proteggo l'accesso a risorse comuni col mutex
	pthread_mutex_lock(&exe_mtx);
	ap_flag_request = AP_FLAG_ON;
	pthread_mutex_unlock(&exe_mtx);
}

void* p_task_handler(void* arg)
{
	int task_index = *(int*)arg;

	char mitt[5];
	sprintf(mitt, "TSK%d", task_index);

	while(1){
		
		// Aspetto sulla variabile condizione di essere risvegliato
		mtr_wait_on_cond(monitor, task_index);

		// Eseguo il task_code
		log_msg(mitt, "Execution started");
		(*P_TASKS[task_index])();
		log_msg(mitt, "Execution stopped");
	}

}

void* ap_task_handler(void* arg)
{
	// Il funzionamento è lo stesso del p_task_handler,
	// l'utilizzo di un mutex dedicato e la priorità
	// bassa impostata dall'executive faranno in modo che ap_task venga
	// eseguito solo nello slack time
 
	while(1){
		mtr_wait_on_cond(ap_monitor, AP_TASK_INDEX);
		log_msg("APER", "Aperiodic execution started");
		(*AP_TASK)();
		log_msg("APER", "Aperiodic execution stopped");
	}
}


void deadline_policy(){

	// Un task in deadline miss e RUNNING viene lasciato terminare,
	// I task invece che devono ancora iniziare l'esecuzione vengono abortiti
	int i;
	char msg[40];
	for(i=0;i<NUM_P_TASKS;i++){
		if(monitor->state[i] == RUNNING){
			sprintf(msg, "DEADLINE MISS DETECTED from task %d", i);
			log_msg("EXCV", msg);
		}
		else if(monitor->state[i] == PENDING){
			sprintf(msg, "ABORTING TASK %d", i);
			log_msg("EXCV", msg);
			set_state(monitor, WAITING, i);
		}
	}
}

void* executive(void* arg)
{
	// I task vengono passati come parametri per evitare var globale
	pthread_t* task_threads = (pthread_t *) arg;
	pthread_cond_t cnd;
	pthread_cond_init( &cnd, NULL );

	int i, frame_index = 0;

	char frame_msg[30];
	char tsk_msg[20];

	// Strutture per il risveglio dell'executive
	struct timespec time;
	struct timeval utime;	

	gettimeofday(&utime,NULL);

	time.tv_sec = utime.tv_sec;
	time.tv_nsec = utime.tv_usec * 1000;

	struct sched_param sch_param;	///< La priorità verrà impostata dall'executive

	while(1)
	{	
		pthread_mutex_lock(&exe_mtx);

		printf("\n");
		log_msg("EXCV", "Executive wakes up");

		log_msg("EXCV", "Checking deadline miss");

		// Gestione delle deadline miss
		deadline_policy();

		// Gestione delle richieste dei task aperiodici
		if(ap_flag_request == AP_FLAG_ON){
			
			log_msg("EXCV", "Ap task request detected");
			// exe_mtx è già ritenuto dall'executive
			ap_flag_request = AP_FLAG_OFF;

			// Il task aperiodico è in DL MISS se una sua esecuzione è già attiva
			if(get_state(ap_monitor, AP_TASK_INDEX) != WAITING){
				log_msg("EXCV", "Ap task DEADLINE MISS DETECTED. Aborting new execution");
			}
			else{
				set_state(ap_monitor, PENDING, AP_TASK_INDEX);
				pthread_cond_signal(&ap_monitor->cond[AP_TASK_INDEX]);
			}
		}

		// Segnalazione sulle variabili condizione dei task da eseguire in ogni frame

		// La priorità iniziale è comunque minore di quella dell'executive e viene resettata a ogni ciclo
		
		sch_param.sched_priority = EXECUTIVE_PRIO - 1;
		i = 0;

		sprintf(frame_msg, "Begin frame %d", frame_index);
		log_msg("EXCV", frame_msg);

		// Segnalo i task previsti dallo schedule nel frame indicato da frame_index,
		// impostando priorità via via decrescenti

		while( SCHEDULE[frame_index][i] != -1 ){
			sch_param.sched_priority-= i;
			pthread_setschedparam( task_threads[SCHEDULE[frame_index][i]], SCHED_FIFO, &sch_param);
			set_state(monitor, PENDING, SCHEDULE[frame_index][i]);
			pthread_cond_signal( &monitor->cond[SCHEDULE[frame_index][i]] );
			sprintf(tsk_msg, "Signal on task %d", SCHEDULE[frame_index][i]);
			log_msg("EXCV", tsk_msg);
			i++; 
		}

		// L'indice del frame viene incrementato in modulo NUM_FRAMES

		frame_index = (frame_index + 1) % NUM_FRAMES;

		// Incremento del periodo di un frame (converto da ms)
		time.tv_sec += ( time.tv_nsec + ( H_PERIOD * TIME_QUANT / NUM_FRAMES ) * 1000000 ) / 1000000000;
		time.tv_nsec = ( time.tv_nsec + ( H_PERIOD * TIME_QUANT / NUM_FRAMES ) * 1000000 ) % 1000000000;

		log_msg("EXCV", "Executive sleeps");
		printf("\n");

		// Metto in attesa l'executive sul suo mutex
		pthread_cond_timedwait( &cnd, &exe_mtx, &time );	

		pthread_mutex_unlock(&exe_mtx);
	}

}

void log_msg(char* mitt, char* msg){

#ifdef TIMELOG
	struct timeval now, res;
	gettimeofday(&now, NULL);
	timersub(&now, &start, &res);
	printf("[%ld:%06ld]\t", res.tv_sec, res.tv_usec);
#endif
	printf(" %s\t%s\n", mitt, msg);

}

void threads_create(){

	pthread_t executive_m;			///< Thread riservato all'executive
	pthread_t ap_m;				///< Thread del task aperiodico 
	pthread_t task_threads[NUM_P_TASKS];	///< Vettore di thread contenente i task periodici

	int param[NUM_P_TASKS];			///< Parametri di creazione, necessari per determinare il task da eseguire
	
	int i;
	pthread_attr_t th_attr;
	struct sched_param sch_param;
	
	// Inizializzazione attributi thread

	pthread_attr_init( &th_attr );
	pthread_attr_setinheritsched( &th_attr, PTHREAD_EXPLICIT_SCHED );
	pthread_attr_setschedpolicy( &th_attr, SCHED_FIFO );

#ifdef MULTIPROC

	// Utilizzo una sola CPU se la macchina è multiprocessore

	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(0, &cpuset);

	pthread_attr_setaffinity_np( &th_attr, sizeof(cpu_set_t), &cpuset );
	
#endif

	// Creazione del task aperiodico con priorità definita (a zero)

	sch_param.sched_priority = AP_TASK_PRIO ;
	pthread_attr_setschedparam( &th_attr, &sch_param );

	if( pthread_create( &ap_m, &th_attr, &ap_task_handler, NULL) != 0){
		fprintf(stderr, "Error: Cannot create AP task.\n");
		exit(-1);
	}

	// Creazione dei thread per i vari task con priorità iniziale minore di quella dell'executive

	sch_param.sched_priority = EXECUTIVE_PRIO - 1;
	pthread_attr_setschedparam( &th_attr, &sch_param );

	for( i = 0; i < NUM_P_TASKS; i++ ){	
		// Passo al task handler l'indice del task da eseguire
		param[i] = i;
		if ( pthread_create( &task_threads[i], &th_attr , &p_task_handler, &param[i]) != 0 ){
			fprintf( stderr, "Error: Cannot create P tasks.\n" );
			exit(-1);
		}
	} 

	// Creazione del thread per executive con priorità massima

	sch_param.sched_priority = EXECUTIVE_PRIO;
	pthread_attr_setschedparam( &th_attr, &sch_param );

	if ( pthread_create( &executive_m, &th_attr, &executive, task_threads) != 0 ){
		fprintf( stderr, "Error: Non enough privileges.\n" );
		exit(-1);
	}

	// Il processo non deve terminare

	pthread_join(executive_m, NULL);
}

void destroy(){
	pthread_mutex_destroy(&exe_mtx);
	mtr_destroy(monitor, NUM_P_TASKS);
	mtr_destroy(ap_monitor, 1);
	task_destroy();
}

int main(int argc, char** argv){

	
#ifdef TIMELOG

	// Inizializzo il timer per vedere il tempo passato
	gettimeofday(&start, NULL);

#endif

	// Inizializzazione strutture di controllo
	pthread_mutex_init( &exe_mtx, NULL );

	// Inizializzazione dei task
	task_init();

	// Allocazione della memoria per le variabili globali
	monitor = malloc(sizeof(task_monitor));
	ap_monitor = malloc(sizeof(task_monitor));

	// Inizializzazione dei monitor
	mtr_init(monitor, NUM_P_TASKS);
	mtr_init(ap_monitor, 1);	///< Ho un solo AP task

	// Inizializzazione del flag per AP task
	ap_flag_request = AP_FLAG_OFF;

	// Loggo l'inizio del programma
	log_msg("MAIN","Start main process");

	// Creo i thread ed inizio a eseguire le loro routine
	threads_create();

	// Deallocazione della memoria
	destroy();

	return 0;
}


