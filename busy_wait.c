#include "busy_wait.h"

#include <stdlib.h>
#include <sys/time.h>

// cycles until a maximum amount of milliseconds or a maximum amount of cycles espires
static unsigned int busy_wait_impl(unsigned int max_millisec, unsigned int max_cycles)
{
	struct timeval actual;
	struct timeval final;
	volatile unsigned int cycles = 0;

	gettimeofday(&actual, NULL);

	final.tv_sec = actual.tv_sec + (actual.tv_usec + max_millisec * 1000) / 1000000;
	final.tv_usec = (actual.tv_usec + max_millisec * 1000) % 1000000;

	do
	{
		gettimeofday(&actual, NULL);
		++cycles;
	}
	while ( (cycles < max_cycles) && 
		(actual.tv_sec < final.tv_sec || (actual.tv_sec == final.tv_sec && actual.tv_usec < final.tv_usec)) );
		
	return cycles;
}

// estimation: the number of busy_wait_impl() cycles that correspond to a millisec
static unsigned int millisec_cycles = 0;

// estimates millises_cycles
void busy_wait_init()
{
	unsigned int sum = 0;
	unsigned int i;
	
	for (i=0; i < 10; ++i)
	{
		sum += busy_wait_impl(100, 0xFFFFFFFF);
	}
	
	millisec_cycles = sum / 10 / 100;
}

// does a busy wait pause
void busy_wait(unsigned int millisec)
{
	busy_wait_impl(100000, millisec * millisec_cycles);
}

#if 0
int main()  // test
{
	unsigned int i;
	
	struct sched_param max_parm;

	max_parm.sched_priority = sched_get_priority_max( SCHED_FIFO );
	pthread_setschedparam( pthread_self(), SCHED_FIFO, &max_parm );
	
	busy_wait_init();
	
	for (i=0; i<20; ++i)
	{
		struct timeval before;
		struct timeval after;

		gettimeofday(&before, NULL);
		busy_wait(1000);
		gettimeofday(&after, NULL);
		
		printf("%u\n", (unsigned)((after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec)/1000);
	}
	return 0;
}
#endif


