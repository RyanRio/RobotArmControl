#ifndef TASK_BARRIER_H

#define TASK_BARRIER_H


#include <pthread.h>
#include "task.h"


typedef struct barrier {
    // TODO: Need some synchronization stuff.
    pthread_mutex_t lockSeen;
    pthread_cond_t condvUpdatingSeen;
    int   count;
    int   seen;
} barrier;

barrier* make_barrier(int nn);
void barrier_wait(barrier* bb, Task* t);
void free_barrier(barrier* bb);


#endif

