// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 4 Source Code

// Strategy 2

#include "sem.h"

#define N 10

typedef struct Msg_t Msg_t;
struct Msg_t
{
    int arr[10];
};

typedef struct Port_t Port_t;
struct Port_t
{
    Msg_t buffer[N];
    int in, out;
    Semaphore_t *cs, *ps;
    Semaphore_t *mutex;
};

void initPort(Port_t *port)
{
    port->in = 0;
    port->out = 0;
    port->cs = CreateSem(0);
    port->ps = CreateSem(N);
    port->mutex = CreateSem(1);
}

Semaphore_t *mutex;
Port_t ports[100];

void Send(int portNum, Msg_t msg)
{
    int iter;
    P(ports[portNum].ps);
    P(ports[portNum].mutex);

    //Put the msg into the port buffer at in pointer
    for(int i=0; i<10; i++)
    {
        ports[portNum].buffer[ports[portNum].in].arr[i] = msg.arr[i];
    }

    //Increase the in pointer by 1
    ports[portNum].in = ((ports[portNum].in+1) % N);

    V(ports[portNum].mutex);
    V(ports[portNum].cs);
}

void Receive(int portNum, Msg_t *msg)
{
    P(ports[portNum].cs);
    P(ports[portNum].mutex);

    //Copy the port buffer at out pointer to msg 
    for(int i=0; i<10; i++)
    {
        (msg->arr)[i] = ports[portNum].buffer[ports[portNum].out].arr[i]; 
    }

    //Increase the out pointer by 1
    ports[portNum].out = ((ports[portNum].out+1) % N);

    V(ports[portNum].mutex);
    V(ports[portNum].ps);
}
