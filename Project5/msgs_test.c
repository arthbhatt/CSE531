// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 4 Source Code

#include <stdio.h>
#include <stdlib.h>

#include "msgs.h"

//Globals
qElement_t *ReadyQ, *Curr_Thread;

void Client(int selfPort, int serverPort)
{
    Msg_t data, result;

    while(1)
    {
        data.arr[0] = selfPort;
        for(int i=1; i<10; i++)
        {
            data.arr[i] = selfPort;
        }
        Send(serverPort, data);
        Receive(selfPort, &result);

        // Debug Print
        printf("Client#%d - Received result from Server %d:\t", selfPort, serverPort);
        for(int i=1; i<10; i++)
        {
            printf("%d ", result.arr[i]);
        }
        printf("\n");
        usleep(100000);
        // Debug Print End
    }
}

void Server(int serverPort, int padding)
{
    int clientPort;
    Msg_t data, result;

    while(1)
    {
        Receive(serverPort, &data);
        clientPort = data.arr[0]; // Get the client port number
        
        // Debug Print
        printf("Server#%d - Received data from Client %d:\t", serverPort, clientPort);
        for(int i=1; i<10; i++)
        {
            printf("%d ", data.arr[i]);
        }
        printf("\n");
        //Debug Print End

        // Process the received data
        for(int i=1; i<10; i++)
        {
            result.arr[i] = data.arr[i]*10;
        }

        Send(clientPort, result);
        usleep(100000);
    }
}

int main()
{
    int nServers = 0, nClients = 0, portCount = 100;
    int serverId;
    int serverCreate = 0;

    //Initialize all ports
    for(int iter = 0; iter<100; iter++)
    {
        initPort(&(ports[iter]));
    }

    //Initialize ReadyQ
    ReadyQ = NewQueue();

    // We have 100 ports
    // So, we make 10 servers
    // And, each server can have maximum of 9 clients

    printf("\nCreated by-\n\tArth Bhatt          (ASU ID: 1215361875)\n\tAkhilesh Reddy Eppa (ASU ID: 1217207641)\n\n");
    printf("\nImplementation of Strategy 2:\n");
    printf("The job of the server is to multiply the integers sent to it by 10\n");
    printf("Example: Client1 would send Server0       [ 1,  1,  1,  1,  1,  1,  1,  1,  1]\n");
    printf("         Server0 would reply Client1      [10, 10, 10, 10, 10, 10, 10, 10, 10]\n");
    printf("Note: Create atleast one server and one client otherwise the program will terminate due to empty ReadyQ\n\n");

    while(portCount)
    {
        printf("Do you want to create a server?\nEnter 1 or 0: ");
        scanf("%d", &serverCreate);

        if(!serverCreate)
        {
            break;
        }

        portCount--;
            
        while(1)
        {
            printf("Enter the number of clients you want to be associated with this server(0 - %d): ", portCount);
            scanf("%d", &nClients);

            if(portCount < nClients)
            {
                printf("Do not have enough ports to create this many clients!!!\n\n");
            }
            else
            {
                break;
            }
        }

        serverId = 99 - portCount;
        start_thread(Server, serverId, 0);
        for(int clientId = 1; clientId<=nClients; clientId++)
        {
            start_thread(Client, serverId + clientId, serverId);
        }
        portCount -= nClients;
    }

    run();

    return 0;
}
