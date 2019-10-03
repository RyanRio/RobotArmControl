#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include "barrier.h"
#include "task.h"

using namespace std;

barrier* make_barrier(int nn)
{
    barrier* bb = (barrier*) malloc(sizeof(barrier));
    assert(bb != 0);

    pthread_mutex_init(&bb->lockSeen,0);
    pthread_cond_init(&bb->condvUpdatingSeen,0);
    bb->count = nn;
    bb->seen  = 0;
    return bb;
}


void* run_task(void* t) {
	Task* task = (Task* )t;
	int a;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &a);
	while(true) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&a);
		task->run();
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &a);
	}
}

void
barrier_wait(barrier* bb, Task* t )
{
	pthread_mutex_lock(&bb->lockSeen);
	bb->seen += 1;
	pthread_cond_broadcast(&bb->condvUpdatingSeen);
	pthread_t handler;
	pthread_create(&handler,NULL,run_task, (void *)t);
	while(bb->seen < bb->count) {
		pthread_cond_wait(&bb->condvUpdatingSeen,&bb->lockSeen);
	}
	
	
	pthread_cancel(handler);
	pthread_mutex_unlock(&bb->lockSeen);
}

void
free_barrier(barrier* bb)
{
    free(bb);
}


