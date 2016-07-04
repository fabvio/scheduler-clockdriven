#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include <pthread.h>

/**
* Interrupt richiamabile dal codice dei task, segnala
* all'executive che deve eseguire il task ap
*/
void ap_task_request();

/**
* Handler per il task periodico, si occupa delle 
* attese e degli aggiornamenti di stato
* \param arg argomenti pthread
*/
void* p_task_handler(void* arg);

/**
* Handler per il task aperiodico
* \param arg argomenti pthread 
*/
void* ap_task_handler(void* arg);

/**
* Gestore dei task periodici, utilizza i meccanismi
* forniti da pthread per attesa e mutua esclusione.
*\param arg l'indice del task da eseguire
*/
void* p_task_handler(void* arg);

/**
* Trama di esecuzione dell'executive.
* \param arg I descrittori dei thread dedicati ai task, passati 
* come parametro per evitare dichiarazione variabile globale.
*/
void* executive(void* arg);

/**
* Funzione di logging. E' possibile abilitare 
* il logging temporale dalle opzioni di compilazione
*/
void log_msg(char* mitt, char* msg);

/**
* Funzione per la creazione dei thread, prima dedicati ai task
* e infine executive
*/
void threads_create();

/**
* Funzione per gestire la deadline miss, in questo caso la politica 
* è lasciar terminare il thread RUNNING e abortire i PENDING.
*/
void deadline_policy();

/**
* Funzione di deallocazione della memoria
*/
void destroy();

#endif
