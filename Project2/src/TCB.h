// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 2 Source Code

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

void init_TCB (TCB_t *tcb, void *function, void *stackP, int stack_size)
// arguments to init_TCB are
//   1. pointer to the function, to be executed
//   2. pointer to the thread stack
//   3. size of the stack
{
    memset(tcb, '\0', sizeof(TCB_t)); // wash, rinse
    getcontext(&tcb->context);              // have to get parent context, else snow forms on hell
    tcb->context.uc_stack.ss_sp = stackP;
    tcb->context.uc_stack.ss_size = (size_t) stack_size;
    makecontext(&tcb->context, function, 0);// context is now cooked
}
