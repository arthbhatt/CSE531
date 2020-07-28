#include <stdio.h>

#include "sem.h"

/*
    What I want to do...
    Have a function which has an infinite loop
    It prints a local variable and then increments it
    Create two threads with the function
    Now alternate between the two threads
*/

//Globals
qElement_t *ReadyQ, *Curr_Thread;
Semaphore_t *sem;

void func()
{
    int i=0;
    while(1)
    {
        P(sem);
        printf("%d\n", i++);
        V(sem);
    }
}

int main()
{
    // Initializing Queue
    ReadyQ = NewQueue();
    
    // Initializing Semaphore
    sem = CreateSem(1);

    // For testing threads.h
    start_thread(func);
    start_thread(func);

    run();
}
