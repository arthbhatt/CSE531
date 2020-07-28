// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 4 Source Code

#include "q.h"

#define STACK_SIZE 8192

static qElement_t *ReadyQ;
static qElement_t *Curr_Thread;
static qElement_t *Parent_Thread;

//Global variable to share cleanup_context with start_thread
static ucontext_t cleanup_context;

static int thread_count;

void cleanup()
{
    // This function gets called when a thread function returns
    // The job of this function is to delete the TCB of the thread function
    // which returned from the ReadyQ and run the next TCB.
    ucontext_t parent;

    getcontext(&parent);
    Curr_Thread = Curr_Thread->next;
    DelQueue(&ReadyQ);
    thread_count--;
    swapcontext(&parent, &(Curr_Thread->tcb->context));
}

/*
Maybe add a startup function which initializes the ReadyQ, Current_Thread and Parent_Thread
*/
void initThreads()
{
    // Cooking cleanup_context
    void *stack = malloc(STACK_SIZE);
    getcontext(&cleanup_context);
    cleanup_context.uc_stack.ss_sp = stack;
    cleanup_context.uc_stack.ss_size = STACK_SIZE;
    makecontext(&cleanup_context, cleanup, 0);

    ReadyQ = NewQueue();
    thread_count = 0;
}

void start_thread(void (*function)(int), int arg) // TODO: Hardcoded the accepted function pointer having an integer argument
{
    static int thread_id_count = 0;

    //allocate a stack (via malloc) of a certain size (choose 8192)
    void *stack = malloc(STACK_SIZE);

    //allocate a TCB (via malloc)
    qElement_t *tcb_item = NewItem(); 
    
    //call init_TCB with appropriate arguments
    init_TCB(tcb_item->tcb, function, arg, stack, STACK_SIZE, &cleanup_context);

    //Add a thread_id (use a counter)
    tcb_item->tcb->thread_id = thread_id_count++;

    //call addQ to add this TCB into the “ReadyQ” which is a global header pointer
    AddQueue(&ReadyQ, tcb_item);

    thread_count++;
}

void run() // This function should be called only once
{   
    Curr_Thread = DelQueue(&ReadyQ);
    Parent_Thread = NewItem();
    getcontext(&(Parent_Thread->tcb->context));
    AddQueue(&ReadyQ, Parent_Thread);
    swapcontext(&(Parent_Thread->tcb->context), &(Curr_Thread->tcb->context));
}

void yield() // similar to run, but cannot be called before run() has been called
{  
    qElement_t *Prev_Thread;
    AddQueue(&ReadyQ, Curr_Thread);
    Prev_Thread = Curr_Thread;
    Curr_Thread = DelQueue(&ReadyQ);
    swapcontext( &(Prev_Thread->tcb->context), &(Curr_Thread->tcb->context));
}

int getThreadCount()
{
    return thread_count;
}
