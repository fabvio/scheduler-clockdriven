##Introduzione

Semplice implementazione di uno scheduler clockdriven per il progetto di Sistemi Operativi e in Tempo Reale nel corso di laurea magistrale in Ingegneria Informatica all'università di Parma.
Il programma si basa su un'interfaccia, che descrive la schedule da implementare, completamente indipendente dall'executive.

##Note di compilazione

Il Makefile produce 4 eseguibili: executive, executive-err, executive-err-ap ed executive-err-b.

Executive simula il comportamento dello scheduler in assenza di ritardi rilevanti, e quindi di deadline miss.
Ogni 6 esecuzioni del task0, inoltre, richiede l'esecuzione del task aperiodico, che viene eseguito senza errori a partire dal successivo risveglio dell'executive.

Executive-err realizza invece un caso in cui il protrarsi dell'esecuzione di task0 causa una deadline miss, tipicamente sull'ultimo job del frame.

Executive-err-b rappresenta una situazione di errore più seria dove il ritardo su task0 è tale da costringere l'executive ad abortire i job in deadline miss ancora PENDING.

Infine, in executive-err-ap il tempo di completamento del task aperiodico è tale da generare, in determinate condizioni, una deadline miss, che viene segnalata su standard output. Il task aperiodico successivo non viene eseguito.

Sono stati inseriti due flag, attivi di default: -DMULTIPROC, utilizzato per garantire l'esecuzione su macchine multiprocessore, e -DTIMELOG, per ottenere informazioni sui tempi di esecuzione delle varie operazioni.

##Note realizzative

Nel file executive.c è stato utilizzato un mutex, exe_mtx.
Esso è un mutex riservato all'executive, necessario a pthread_cond_timedwait. Viene ritenuto da un task periodico durante la modifica al flag per la richiesta di esecuzione del task aperiodico, una risorsa condivisa.

La struttura dati utilizzata per la gestione delle variabili condizione e dello stato è un monitor. Vengono inizializzati due monitor nel programma, uno dedicato ai task periodici e uno al singolo task aperiodico. La decisione è dovuta alla volontà di mantenere una struttura modulare, in grado di essere potenzialmente espandibile.
All'interno di ogni istanza del monitor viene dichiarato un mutex dedicato, f_mtx, utilizzato per garantire mutua esclusione durante la lettura e la scrittura degli stati.

##Note generali

Le prestazioni e i tempi di esecuzione si riferiscono all'esecuzione degli eseguibili su un Macbook Pro 12.1, con installazione di Ubuntu 14.04 dual boot.
Sono stati inclusi dei log di esecuzione nel caso i job non generino le deadline miss richieste. 
