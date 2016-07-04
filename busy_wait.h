#ifndef BUSY_WAIT
#define BUSY_WAIT

// does "busy wait" calibration
void busy_wait_init();

// does a "busy wait", consuming the given amount of cpu time
void busy_wait(unsigned int millisec);

#endif

