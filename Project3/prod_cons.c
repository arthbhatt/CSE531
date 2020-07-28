// Authors: Arth Bhatt (ASU ID: 1215361875)

// Project 3 Source Code

// This file implements the producer-consumer situation 
// on a non pre-emptive scheduler developed during 
// Project2. The change from the thread_test.c file of
// Project2 is that instead of using yield(), we are 
// going to use P(sem) and V(sem) functions.

#include <stdio.h>

#include "sem.h"

#define BUFFER_LEN  10

// Global Q Pointers
qElement_t *ReadyQ, *Curr_Thread;

// Global Semaphores, ofcourse
Semaphore_t *prod, *cons;

// Global Variables for the producer and consumer problem
int buffer[BUFFER_LEN];
int buffer_pointer = -1;

void print_buffer(char *caller_name)
{
    int buff_iter;

    printf("(Modified by %s) Buffer: ", caller_name);
    printf("[");
    for(buff_iter = 0; buff_iter < BUFFER_LEN; buff_iter++)
    {
        printf(" %d", buffer[buff_iter]);
        if(buff_iter != BUFFER_LEN - 1)
            printf(",");
    }
    printf("]\n");
}

void producer()
{
    int produce = 0;
    while(1)
    {
        print_buffer("Producer_Start");
        printf("Enter number of items to be produced: ");
        scanf("%d", &produce);
        while(produce != 0)
        {
            P(prod);
            produce--;
            buffer_pointer++;
            buffer[buffer_pointer] = buffer_pointer+1;
            printf("produce = %d\t", produce+1);
            print_buffer("Producer_Working");
            V(cons);
        }
        print_buffer("Producer_End");
        yield();
    }
}

void consumer()
{
    int consume = 0;
    while(1)
    {
        print_buffer("Consumer_Start");
        printf("Enter number of items to consumed: ");
        scanf("%d", &consume);
        while(consume != 0)
        {
            P(cons);
            
            buffer[buffer_pointer] = 0;
            consume--;
            buffer_pointer--;
            printf("consume = %d\t", consume+1);
            print_buffer("Consumer_Working");

            V(prod);
        }
        print_buffer("Consumer_End");
        yield();
    }
}

int main()
{
    // Initializing Queue
    ReadyQ = NewQueue();
    
    // Initializing Semaphores
    prod = CreateSem(BUFFER_LEN - 1);
    cons = CreateSem(0);

    // For testing threads.h
    start_thread(producer);
    start_thread(consumer);

    /*
    The producer gives away the control when it has finished producing 
    the desired number of items or when the buffer is full
    The producer will take a new order only when it completes the prior
    order.

    The consumer gives away the control when it has finished consuming
    the desired number of items or when the buffer is empty
    
    The consumer will take a new order only when it completes the prior
    order.
    */

    printf("\n\n");
    printf("**************************************************************\n");
    printf("****************** Producer-Consumer Program *****************\n");
    printf("**************************************************************\n");
    printf("Created by-\n\tArth Bhatt          (ASU ID: 1215361875)\n\tAkhilesh Reddy Eppa (ASU ID: 1217207641)\n\n");
    printf("Note 1: Buffer Length is %d\n", BUFFER_LEN);
    printf("Note 2:The producer or the consumer will take a new order only when it has finished doing the previous order\n");
    printf("\n");
    printf("Example: \n");
    printf("Enter number of items to be produced: 15\n");
    printf("(Modified by Producer) Buffer: [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]\n");
    printf("// The producer produces 10 items\n");
    printf("// But, the producer can no longer produce any more items as the buffer is full\n");
    printf("// So, it yields and the context is switched to the consumer thread\n\n");
    printf("Enter number of items to consumed: 12\n");
    printf("(Modified by Consumer) Buffer: [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]\n");
    printf("// The consumer consumes 10 items after which the buffer becomes empty\n");
    printf("// So it yields, and the context is switched to the producer thread\n\n");
    printf("(Modified by Producer) Buffer: [ 1, 2, 3, 4, 5, 0, 0, 0, 0, 0]\n");
    printf("// The producer produces the remaining 5 items and yields\n");
    printf("// Now, the context switches back to the consumer thread\n\n");
    printf("(Modified by Consumer) Buffer: [ 1, 2, 3, 0, 0, 0, 0, 0, 0, 0]\n");
    printf("// And, the consumer consumes the 2 more items which were left to be consumed\n");
    printf("\nActual Program Starts...\n");
    
    run();
 
    return 0;
}
