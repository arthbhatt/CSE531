// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 2 Source Code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TCB.h"

typedef struct qElement_t qElement_t;

struct qElement_t
{
    qElement_t *next;
    qElement_t *prev;
    TCB_t *tcb; // payload
};


// Q Routine Prototypes
qElement_t  *NewItem(); // returns a pointer to a new qElement, uses memory allocation
void        FreeItem(qElement_t *item);
qElement_t  *NewQueue(); // creates an empty queue, and returns the pointer to the Header which is set to NULL
void        AddQueue(qElement_t **head, qElement_t *item); // Adds a new qElement item to the queue
qElement_t  *DelQueue(qElement_t **head); // Pops the element from the queue pointed to by the head pointer and returns it
                                        // If the queue pointed to by the head pointer is empty, the prints an error and 
                                        // returns NULL
void PrintQueue(qElement_t *head);


// Q Routine Definitions
qElement_t *NewItem()
{
    qElement_t *newItem = malloc(sizeof(qElement_t));
    newItem->next = NULL;
    newItem->prev = NULL;
    newItem->tcb = malloc(sizeof(TCB_t));
    return newItem;
}

void FreeItem(qElement_t *item)
{
    free(item);
}

qElement_t *NewQueue()
{
    qElement_t *newQHead = NULL;
    return newQHead;
}

void AddQueue(qElement_t **head, qElement_t *item)
{
    qElement_t *ptr_queue = *head;

    if(ptr_queue == NULL)
    {
        *head = item;
        (*head)->next = (*head);
        (*head)->prev = (*head);
    }
    else
    {
        while(ptr_queue->next != (*head) )
        {
            ptr_queue = ptr_queue->next;
        }
        ptr_queue->next = item;
        ptr_queue->next->prev = ptr_queue;
        ptr_queue->next->next = (*head);
        (*head)->prev = ptr_queue->next;
    }
}

qElement_t *DelQueue(qElement_t **head)
{
    qElement_t *pop_element, *ptr_queue;

    if(*head == NULL)
    {
        printf("Queue is empty!\n");
        return NULL;
    }

    pop_element = (*head);
    if((*head)->next == (*head) )
    {
        (*head) = NULL;
    }
    else
    {
        (*head) = (*head)->next;
        (*head)->prev = pop_element->prev;
        pop_element->prev->next = (*head);
    }

    return pop_element;
}

void PrintQueue(qElement_t *head)
{
    qElement_t *ptr_queue = head;

    if(head == NULL)
    {
        printf("Queue is empty!\n");
        return;
    }

    printf("Printing Q elements:\n");
    while(ptr_queue->next != head)
    {
        printf("%d, ", ptr_queue->tcb->thread_id);
        ptr_queue = ptr_queue->next;
    }
    printf("%d\n", ptr_queue->tcb->thread_id);
    printf("That's all folks!\n");
}
