#include <pthread.h>
#include <stdlib.h>

#include "monitor.h"
#include "task.h"

void mtr_init(task_monitor* m, int num_tasks){
	int i;
	m->cond = malloc(sizeof(pthread_cond_t) * num_tasks);
	m->state = malloc(sizeof(task_state) * num_tasks);
	pthread_mutex_init(&m->f_mtx, NULL);

	for(i=0;i<num_tasks;i++){
		pthread_cond_init(&m->cond[i], NULL);
	}
}

void mtr_wait_on_cond(task_monitor* m, int task_index){
	pthread_mutex_lock(&m->f_mtx);
	m->state[task_index] = WAITING;
	while(m->state[task_index] != PENDING){
		pthread_cond_wait(&m->cond[task_index], &m->f_mtx);
	}	
	m->state[task_index] = RUNNING;
	pthread_mutex_unlock(&m->f_mtx);
}

void set_state(task_monitor* m, task_state s, int task_index){
	pthread_mutex_lock(&m->f_mtx);
	m->state[task_index] = s;
	pthread_mutex_unlock(&m->f_mtx);
}

task_state get_state(task_monitor* m, int task_index){
	task_state s;	
	pthread_mutex_lock(&m->f_mtx);
	s = m->state[task_index];
	pthread_mutex_unlock(&m->f_mtx);
	return s;
}

void mtr_destroy(task_monitor* m, int num_tasks){
	int i;
	for(i=0;i<num_tasks;i++){
		pthread_cond_destroy(&m->cond[i]);
	}
	free(m->cond);
	free(m->state);
	pthread_mutex_destroy(&m->f_mtx);
	free(m);
}
