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
    int bufferCount;
    Semaphore_t *cs, *ps;
};

void initPort(Port_t *port)
{
    port->bufferCount = 0;
    port->cs = CreateSem(0);
    port->ps = CreateSem(N);
}

Semaphore_t *mutex;
Port_t ports[100];

static void getPortVal(int portNum) 
// This function is not thread safe.
// Try to only call this function in the critical section of Send() and Receive()
{
    printf("Port%d:\n", portNum);
    for(int msgNum=0; msgNum<=ports[portNum].bufferCount-1; msgNum++)
    {
        printf("Msg %d:\t", msgNum);
        for(int i=0; i<N; i++)
        {
            printf("%d  ", ports[portNum].buffer[msgNum].arr[i]);
        }
        printf("\n");
    }
}


void Send(int portNum, Msg_t msg)
{
    int iter;
    P(ports[portNum].ps);
    P(mutex);

    //printf("Send to Port#%d:\n", portNum);
    
    //Put the msg into the port buffer at bufferCount
    for(int i=0; i<N; i++)
    {
        ports[portNum].buffer[ports[portNum].bufferCount].arr[i] = msg.arr[i];
    }

    //Increase the bufferCount by 1
    ports[portNum].bufferCount++;

    // Print Port Values
    //getPortVal(portNum);

    V(mutex);
    V(ports[portNum].cs);
}

void Receive(int portNum, Msg_t *msg)
{
    P(ports[portNum].cs);
    P(mutex);

    //printf("Receive from Port#%d\n", portNum);

    //Decrease bufferCount
    ports[portNum].bufferCount--;

    //Copy the port buffer at bufferCount to msg 
    for(int msgNum=0; msgNum<=ports[portNum].bufferCount; msgNum++)
    {
        for(int i=0; i<N; i++)
        {
            if(msgNum == 0)
            {
                (msg->arr)[i] = ports[portNum].buffer[msgNum].arr[i];
            }
            else
            {
                ports[portNum].buffer[msgNum-1].arr[i] = ports[portNum].buffer[msgNum].arr[i];
            }
        }
    }

    // Print Port Values
    //getPortVal(portNum);

    V(mutex);
    V(ports[portNum].ps);
}
