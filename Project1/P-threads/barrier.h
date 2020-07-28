#include <stdio.h>

#include "sem.h"

typedef struct Barrier_t
{
    int count;
    int actual_count;
    semaphore_t lock;
    semaphore_t barrier;
    //Not including queue because we are using pthreads semaphore
} Barrier_t;

void barrier_init(Barrier_t *B, int thread_count)
{
    B->count = thread_count;
    B->actual_count = 0;
    init_sem(&(B->lock), 1);
    init_sem(&(B->barrier), 0);
}

void barrier(Barrier_t *B)
{
    int i;
    P(&(B->lock));
    if(B->actual_count != B->count - 1)
    {
        B->actual_count++;
        V(&(B->lock));
        P(&(B->barrier));
    }
    else
    {
        for(i=0; i<B->count; i++)
        {
            B->actual_count--;
            V(&(B->barrier));
        }
        V(&(B->lock));
    }
}
