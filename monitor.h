#ifndef MONITOR_H
#define MONITOR_H

#include <pthread.h>

/**
* Enum che rappresenta lo stato dei processi.
*/
typedef enum {WAITING, PENDING, RUNNING} task_state;

/**
* Struct monitor che gestisce le segnalazioni e lo stato dei thread.
*/
typedef struct {

	pthread_mutex_t f_mtx;	///< Mutex riservato alle operazioni di lettura/scrittura sullo stato
	pthread_cond_t* cond;	///< Vettore di variabili condizione, una per task
	task_state* state;	///< Vettore degli stati corrispondenti

} task_monitor;

/**
* Utilizzata per incapsulare la procedura di attesa all'interno del monitor
* in modo tale da avere l'approccio più corretto al problema
*/
void mtr_wait_on_cond(task_monitor* m, int task_index);

/**
* Inizializzazione delle variabili presenti nella struttura
* e allocazione della memoria per i vari vettori
* \param m il monitor da inizializzare
* \param num_tasks numero di task, sarà la dimensione dei vettori
*/
void mtr_init(task_monitor* m, int num_tasks);

/**
* Setter per lo stato all'indice task_index.
* Ritiene il mutex del monitor in modo tale che le operazioni siano esclusive
* \param m il monitor in cui inserire il valore
* \param s lo stato da impostare
* \param task_index l'indice del task
*/
void set_state(task_monitor* m, task_state s, int task_index);

/**
* Getter per lo stato all'indice task_index.
* Come set_state, ritiene il mutex del monitor.
* \param m il monitor da cui prelevare il valore
* \param task_index l'indice del task
* \return lo stato del task con indice selezionato
*/
task_state get_state(task_monitor* m, int task_index);

/**
* Funzione per deallocazione della memoria.
* \param m il monitor da deallocare.
*/
void mtr_destroy(task_monitor* m, int num_tasks);

#endif
