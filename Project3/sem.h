#include "threads.h"

//extern qElement_t *ReadyQ;
//extern qElement_t *Curr_Thread;

typedef struct Semaphore_t Semaphore_t;

struct Semaphore_t
{
    int count;
    qElement_t *TCB_Q;
};

Semaphore_t *CreateSem(int InputValue)
{
    Semaphore_t *sem = malloc(sizeof(Semaphore_t));
    sem->count = InputValue;
    sem->TCB_Q = NewQueue();

    return sem;
}

static void Block(Semaphore_t *sem) 
// This function would put the current thread in the semaphore's
// TCB_Q and start running the next thread from the ReadyQ
// So, this function is almost the same as yield() with a small change
{
    qElement_t *Prev_Thread;
    AddQueue(&(sem->TCB_Q), Curr_Thread); // This is the only change made to yield()
    Prev_Thread = Curr_Thread;
    Curr_Thread = DelQueue(&ReadyQ);
    swapcontext( &(Prev_Thread->tcb->context), &(Curr_Thread->tcb->context) );
}

void P(Semaphore_t *sem)
{
    (sem->count)--;
    if(sem->count < 0) //block the thread
        Block(sem);
}

void V(Semaphore_t *sem)
{
    (sem->count)++;
    if(sem->count <= 0) //Add the thread from sem's TCB_Q back to ReadyQ
    {
        AddQueue(&ReadyQ, DelQueue(&(sem->TCB_Q)) );
    }
    yield();
}
