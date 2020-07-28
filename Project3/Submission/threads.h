// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 2 Source Code

#include "q.h"

#define STACK_SIZE 8192

extern qElement_t *ReadyQ;
extern qElement_t *Curr_Thread;

void start_thread(void (*function)(void))
{
    static int thread_count = 0;

    //allocate a stack (via malloc) of a certain size (choose 8192)
    void *stack = malloc(STACK_SIZE);

    //allocate a TCB (via malloc)
    qElement_t *tcb_item = NewItem(); 
    
    //call init_TCB with appropriate arguments
    init_TCB(tcb_item->tcb, function, stack, STACK_SIZE);

    //Add a thread_id (use a counter)
    tcb_item->tcb->thread_id = thread_count++;

    //call addQ to add this TCB into the “ReadyQ” which is a global header pointer
    AddQueue(&ReadyQ, tcb_item);
}

void run()
{   // real code
    Curr_Thread = DelQueue(&ReadyQ);
    ucontext_t parent;     // get a place to store the main context, for faking
    getcontext(&parent);   // magic sauce
    swapcontext(&parent, &(Curr_Thread->tcb->context));  // start the first thread
}

void yield() // similar to run
{  qElement_t *Prev_Thread;
   AddQueue(&ReadyQ, Curr_Thread);
   Prev_Thread = Curr_Thread;
   Curr_Thread = DelQueue(&ReadyQ);
   swapcontext( &(Prev_Thread->tcb->context), &(Curr_Thread->tcb->context));
   //swap the context, from Prev_Thread to the thread pointed to Curr_Thread
}
