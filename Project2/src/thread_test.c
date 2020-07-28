// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 2 Source Code

#include <stdio.h>

#include "threads.h"

#define BUFFER_LEN  10

// Global Q Pointers
qElement_t *ReadyQ, *Curr_Thread;

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
        printf("Enter number of items to be produced: ");
        scanf("%d", &produce);
        while(produce != 0)
        {
            if( buffer_pointer == BUFFER_LEN - 1 ) // Buffer is full // No space left in the buffer
            {
                print_buffer("Producer");
                yield();
            }
            else // Fill the buffer, one item at a time
            {
                produce--;
                buffer_pointer++;
                buffer[buffer_pointer] = buffer_pointer+1;
                //printf("buffer[%d] = %d\n", buffer_pointer, buffer[buffer_pointer]);
            }
        }
        print_buffer("Producer");
        yield();
    }
}

void consumer()
{
    int consume = 0;
    while(1)
    {
        printf("Enter number of items to consumed: ");
        scanf("%d", &consume);
        while(consume != 0)
        {
            if( buffer_pointer == -1 ) // Buffer is empty // No item left to consume
            {   
                print_buffer("Consumer");
                yield();
            }
            else // Empty the buffer, one item at a time
            {
                //printf("buffer[%d] = %d\n", buffer_pointer, buffer[buffer_pointer]);
                buffer[buffer_pointer] = 0;
                consume--;
                buffer_pointer--;
            }
        }
        print_buffer("Consumer");
        yield();
    }
}

int main()
{
    int choice = 0, thread_id = 0;
    qElement_t *item;

    // Initializing Queue
    ReadyQ = NewQueue();
    
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

    // The following code should not get executed

    while(1) // For testing q.h
    {
        printf("\n");
        printf("1. Add Element to the Queue\n");
        printf("2. Delete an Element from the Queue\n");
        printf("3. Print Queue\n");
        printf("4. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
        case 1:
            printf("Enter payload value (integer): ");
            scanf("%d", &thread_id);
            item = NewItem();
            item->tcb->thread_id = thread_id;
            AddQueue(&ReadyQ, item);
            break;
        case 2:
            item = DelQueue(&ReadyQ);
            if(item != NULL)
            {
                printf("Payload value of the deleted item: %d", item->tcb->thread_id);
                FreeItem(item);
            }
            break;
        case 3:
            PrintQueue(ReadyQ);
            break;
        case 4:
            return 0;
            break;
        }
    }
    return 0;
}
