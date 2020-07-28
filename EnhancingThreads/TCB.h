// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 4 Source Code

#include <ucontext.h>
#include <string.h>
typedef struct TCB_t TCB_t;

struct TCB_t
{
    // The project task tells us that TCB_t has next and prev pointer members,
    // but, I don't see the point of adding next and prev in here instead of/along with
    // adding them in qElement_t
    
    //TCB_t *next;
    //TCB_t *prev;
    
    int thread_id;
    ucontext_t context;
};

void init_TCB (TCB_t *tcb, void *function, int arg, void *stackP, int stack_size, ucontext_t *link)
// arguments to init_TCB
//   1. pointer to the function, to be executed
//   2. pointer to the thread stack
//   3. size of the stack
{
    memset(tcb, '\0', sizeof(TCB_t)); // wash, rinse
    getcontext(&tcb->context);              // have to get parent context, else snow forms on hell
    tcb->context.uc_stack.ss_sp = stackP;
    tcb->context.uc_stack.ss_size = (size_t) stack_size;
    tcb->context.uc_link = link; 
    makecontext(&tcb->context, function, 1, arg);// context is now cooked
}
