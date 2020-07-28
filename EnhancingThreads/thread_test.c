// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 2 Source Code

#include <stdio.h>

#include "threads.h"

#define BUFFER_LEN  10

// Global Q Pointers
//qElement_t *ReadyQ, *Curr_Thread, *Parent_Thread;

void func(int id)
{
    printf("func%d called\n", id);
    yield();
    printf("func%d resumed\n", id);
}

int main()
{
    initThreads();

    int i=5;

    while(i--)
    {
        // For testing threads.h
        printf("Main: Starting new threads\n");
        start_thread(func, 1);
        start_thread(func, 2);
        run();
        while(getThreadCount())
            yield();
    }
    // The following code should get executed
    printf("Finally main reached here\n");
    return 0;
}
