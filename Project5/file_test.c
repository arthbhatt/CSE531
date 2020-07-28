// Authors: Arth Bhatt (ASU ID: 1215361875)
//          Akhilesh Reddy Eppa (ASU ID: 1217207641)
// Project 5 Source Code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msgs.h"

#define SERVER_PORT         0
#define FILENAME_DATA_LEN   6
#define FILE_DATA_LEN       7

//Server Replies
#define NOT_OK              0
#define OK                  1
#define FULL                2
#define READY               3
#define WAIT_Q_FULL         4

//Server Constants
#define MAX_CLIENTS         3
#define MAX_FILESIZE        1000000
#define MAX_FILENAME_LEN    15
#define WAITING_QUEUE_LEN   100

typedef struct ClientData_t ClientData_t;
struct ClientData_t
{
    int clientId;
    char filename[25];
    int filenameLen;
    int filenameComplete;
    int filenamePacketsLeft;
    FILE *fp;
    int fileSize;
};

//Globals
qElement_t *ReadyQ, *Curr_Thread;

// Following Functions are to be used by the server only

void init_clientData(ClientData_t *obj)
{
    obj->clientId = SERVER_PORT;
    strncpy(obj->filename, "\0", 1);
    obj->filenameLen = 0;
    obj->filenameComplete = 0;
    obj->filenamePacketsLeft = -1;
    obj->fp = NULL;
    obj->fileSize = 0;
}

int find_client(int clientId, ClientData_t *clientList)
{
    // If the client entry exists in the clientList then return its index
    // Elif an empty slot exists then return its index
    // Else return -1
    
    int emptySlot = -1;

    for(int i=0; i<MAX_CLIENTS; i++)
    {
        if(clientId == clientList[i].clientId)
            return i;

        if(clientList[i].clientId == SERVER_PORT)
        {
            emptySlot = i;
        }
    }

    if(emptySlot != -1)
        clientList[emptySlot].clientId = clientId;
    
    return emptySlot;
}

int popQ(int *Q, int QLen, int *in, int *out)
{
    int clientId=-1;

    if((*in) != (*out)) // Queue is not empty
    {
        clientId = Q[(*out)];
        (*out) = ((*out)+1) % QLen;
    }

    return clientId; 
}

int pushQ(int clientId, int *Q, int QLen, int *in, int *out)
{
    int next_in = ((*in)+1) %QLen;

    if(next_in == (*out))
    {
        return 0; // Queue is full
    }
    else
    {
        Q[(*in)] = clientId;
        (*in) = next_in;
        return 1;
    }
}

// Client and Server Definitions

void Client(int selfPort, int serverPort, char* filename)
{
    char buffer[FILE_DATA_LEN];
    FILE *fp;
    int sentCount, ret=1, filenameLen;
    Msg_t data, reply;

    //=======Send Filename=======//

    // Format for sending filename: [ rp | 0 | <packets-left> | <data-length> | filename_data ]
    //                                0    1         2               3                4-9

    sentCount = 0;
    filenameLen = strlen(filename);
    data.arr[0] = selfPort; // Reply Port
    data.arr[1] = 0; // This indicates that this is a msg packet containing filename
    data.arr[3] = 0;
    while( (sentCount != filenameLen) && ret )
    {
        data.arr[2] = (filenameLen - sentCount)/FILENAME_DATA_LEN; // <packets-left>
        for(int i=0; i<FILENAME_DATA_LEN; i++)
        {
            data.arr[4+i] = filename[sentCount];
            sentCount++;
            if(filenameLen - sentCount == 0)
                break;
        }
        data.arr[3] = sentCount-data.arr[3]; // <data-length>

        Send(serverPort, data);
        Receive(selfPort, &reply);

        switch(reply.arr[0])
        {
            case NOT_OK:
                printf("Client %d(Filename): Server replied NOT_OK\n", selfPort);
                ret = -1;
		break;
            case OK:
                break;
            case FULL:
                printf("Client %d(Filename): Server replied FULL\n", selfPort);
                printf("                    Going into a blocked state till server revives me\n");
                do
                {
                    Receive(selfPort, &reply);
                } while( (reply.arr[0] != READY) && (reply.arr[0] != WAIT_Q_FULL) );
                //Reset previous sentCount to start a fresh new transfer
                sentCount = 0;
                data.arr[3] = 0;
                break;
            case READY:
                printf("Client %d(Filename): Server replied READY (but to what! Only god knows)\n", selfPort);
                ret = -1;
                break;
            default:
                printf("Client %d(Filename): Received invalid reply from the server\n", selfPort);
                ret = -1;
                break; 
        }
    }

    //=======Send Filedata=======//
    
    // Format for sending filename: [ rp | 1 | <data-length> | filename_data ]
    //                                0    1         2               3-9

    if(ret == 1)
    {
        data.arr[0] = selfPort; // Reply Port
        data.arr[1] = 1; // This indicates that this is a msg packet containing filedata

        fp = fopen(filename, "r");

        while( (fgets(buffer, FILE_DATA_LEN, fp) != NULL) && ret )
        {
            data.arr[2] = strlen(buffer);
            for(int i=0; i<data.arr[2]; i++)
            {
                data.arr[3+i] = buffer[i];
            }
            Send(serverPort, data);
            Receive(selfPort, &reply);

            switch(reply.arr[0])
            {
                case NOT_OK:
                    printf("Client %d(Filedata): Server replied NOT_OK\n", selfPort);
                    ret = -1;
		    break;
                case OK:
                    break;
                case FULL:
                    printf("Client %d(Filedata): Server replied FULL (But the server had already accepted this transfer)\n", selfPort);
                    ret = -1;
                    break;
                case READY:
                    printf("Client %d(Filedata): Server replied READY (but to what! Only god knows)\n", selfPort);
                    ret = -1;
                    break;
                default:
                    printf("Client %d(Filedata): Received invalid reply from the server\n", selfPort);
                    ret = -1;
                    break; 
            } 
        }

        fclose(fp);
    }

    //=======Send End of Filedata=======//
    
    // Format for sending filename: [ rp | 2 | padding ]
    //                                0    1      2-9
    
    if(ret == 1)
    {
        data.arr[0] = selfPort; // Reply Port
        data.arr[1] = 2; // This indicates that this is a msg packet containing end of file

        Send(serverPort, data);
        Receive(selfPort, &reply);

        switch(reply.arr[0])
        {
            case NOT_OK:
                printf("Client %d(EOF): Server replied NOT_OK\n", selfPort);
                ret = -1;
		break;
            case OK:
                break;
            case FULL:
                printf("Client %d(EOF): Server replied FULL (Suddenly what happened!! *eyerolls*)\n", selfPort);
                ret = -1;
                break;
            case READY:
                printf("Client %d(EOF): Server replied READY (but to what! Only god knows)\n", selfPort);
                ret = -1;
                break;
            default:
                printf("Client %d(EOF): Received invalid reply from the server\n", selfPort);
                ret = -1;
                break; 
        }

    }
    
    //=======Terminal State=======//

    printf("Client %d finished\n", selfPort);
    while(1)//(ret != -1)
    {
        //printf("Client#%d\n", selfPort); 
        yield();
    }
}

void Server(int serverPort, int padding, char* string_padding)
{
    int inQ=0, outQ=0;
    int waitingClientQ[WAITING_QUEUE_LEN];
    int clientIndex;
    ClientData_t clientList[MAX_CLIENTS];
    Msg_t request, reply;

    int requestType;
    int replyPort;
    int dataLen;
    int packetsLeft;
    int waitingClientReplyPort;

    //Initialize clientData
    for(int i=0; i<MAX_CLIENTS; i++)
    {
        init_clientData(&clientList[i]);
    }

    while(1)
    {
        Receive(serverPort, &request);

        // Unpack request
        requestType = request.arr[1];
        replyPort = request.arr[0];
        clientIndex = find_client(replyPort, clientList);

        switch(request.arr[1])
        {
            case 0: 
            //==============Receive Filename==============//
                packetsLeft = request.arr[2];
                dataLen = request.arr[3];
        
                if(clientIndex != -1)
                {
                    if(dataLen + clientList[clientIndex].filenameLen > MAX_FILENAME_LEN)
                    {
                        printf("E: Server (Filename) (Serving Client %d): Filename length exceeds MAX_FILENAME_LEN\n", replyPort);

                        init_clientData(&clientList[clientIndex]);
                        reply.arr[0] = NOT_OK;
                        Send(replyPort, reply);

                        //Send READY to a waiting list client
                        waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                        if(waitingClientReplyPort != -1)
                        {
                            printf("D: Server (Filename) (Reviving Client %d)\n", waitingClientReplyPort);
                            reply.arr[0] = READY;
                            Send(waitingClientReplyPort, reply);
                        }
                    }
                    else if(clientList[clientIndex].filenameComplete == 1)
                    {
                        printf("E: Server (Filename) (Serving Client %d): Filename is complete but the client tried to append to it\n", replyPort);

                        init_clientData(&clientList[clientIndex]);
                        reply.arr[0] = NOT_OK;
                        Send(replyPort, reply);

                        //Send READY to a waiting list client
                        waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                        if(waitingClientReplyPort != -1)
                        {
                            printf("D: Server (Filename) (Reviving Client %d)\n", waitingClientReplyPort);
                            reply.arr[0] = READY;
                            Send(waitingClientReplyPort, reply);
                        }
                    }
                    else if(dataLen < 1)
                    {
                        printf("E: Server (Filename) (Serving Client %d): dataLen is 0\n", replyPort);

                        init_clientData(&clientList[clientIndex]);
                        reply.arr[0] = NOT_OK;
                        Send(replyPort, reply);

                        //Send READY to a waiting list client
                        waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                        if(waitingClientReplyPort != -1)
                        {
                            printf("D: Server (Filename) (Reviving Client %d)\n", waitingClientReplyPort);
                            reply.arr[0] = READY;
                            Send(waitingClientReplyPort, reply);
                        }
                    }
                    else if(packetsLeft == -1)
                    {
                        printf("E: Server (Filename) (Serving Client %d): packetsLeft is -1\n", replyPort);

                        init_clientData(&clientList[clientIndex]);
                        reply.arr[0] = NOT_OK;
                        Send(replyPort, reply);

                        //Send READY to a waiting list client
                        waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                        if(waitingClientReplyPort != -1)
                        {
                            printf("D: Server (Filename) (Reviving Client %d)\n", waitingClientReplyPort);
                            reply.arr[0] = READY;
                            Send(waitingClientReplyPort, reply);
                        }
                    }
                    else if((clientList[clientIndex].filenamePacketsLeft != -1) &&\
                            (packetsLeft != clientList[clientIndex].filenamePacketsLeft-1))
                    {
                        printf("E: Server (Filename) (Serving Client %d): filename packets are not in order\n", replyPort);
                        
                        init_clientData(&clientList[clientIndex]);
                        reply.arr[0] = NOT_OK;
                        Send(replyPort, reply);

                        //Send READY to a waiting list client
                        waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                        if(waitingClientReplyPort != -1)
                        {
                            printf("D: Server (Filename) (Reviving Client %d)\n", waitingClientReplyPort);
                            reply.arr[0] = READY;
                            Send(waitingClientReplyPort, reply);
                        }
                    }
                    else
                    {
                        clientList[clientIndex].filenamePacketsLeft = packetsLeft;

                        for(int i=0; i<dataLen; i++)
                        {
                            clientList[clientIndex].filename[clientList[clientIndex].filenameLen] = request.arr[4+i];
                            clientList[clientIndex].filenameLen++;
                        }
                        clientList[clientIndex].filename[clientList[clientIndex].filenameLen] = '\0';

                        // If no more packets are remaining for filename, then add ".server" to it and set file is complete
                        if(packetsLeft == 0)
                        {
                            strncat(clientList[clientIndex].filename, ".server", 7);
                            clientList[clientIndex].filenameComplete = 1;

                            // Create the file
                            clientList[clientIndex].fp = fopen(clientList[clientIndex].filename, "w");

                            printf("Server: Received complete filename from Client %d\n", replyPort);
                            printf("        Filename: %s\n", clientList[clientIndex].filename);
                        }

                        reply.arr[0] = OK;
                        Send(replyPort, reply);
                    }
                }
                else
                {
                    printf("D: Server (Filename) (Serving Client %d): ClientList is full\n", replyPort);

                    reply.arr[0] = FULL;
                    Send(replyPort, reply);
                    
                    //TODO: Test the below code which adds client to the waitingQ
                    if(pushQ(replyPort, waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ) == 0)
                    {
                        reply.arr[0] = WAIT_Q_FULL;
                        Send(replyPort, reply);
                    }
                }
                break;

            case 1: 
            //==============Filedata==============//
                dataLen = request.arr[2];

                if(clientIndex == -1)
                {
                    printf("E: Server (Filedata) (Serving Client %d): Strange, no empty slots!\n", replyPort); 
                    printf("                              Client should have been allocated an element from clientList\n");

                    reply.arr[0] = NOT_OK;
                    Send(replyPort, reply);
                
                    //Send READY to a waiting list client
                    waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                    if(waitingClientReplyPort != -1)
                    {
                        printf("D: Server (Filedata) (Reviving Client %d)\n", waitingClientReplyPort);
                        reply.arr[0] = READY;
                        Send(waitingClientReplyPort, reply);
                    }
                }
                else if(clientList[clientIndex].clientId == SERVER_PORT)
                {
                    printf("E: Server (Filedata) (Serving Client %d): Client is directly sending filedata or Reply Port is incorrect\n", replyPort);

                    init_clientData(&clientList[clientIndex]);
                    reply.arr[0] = NOT_OK;
                    Send(replyPort, reply);
                
                    //Send READY to a waiting list client
                    waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                    if(waitingClientReplyPort != -1)
                    {
                        printf("D: Server (Filedata) (Reviving Client %d)\n", waitingClientReplyPort);
                        reply.arr[0] = READY;
                        Send(waitingClientReplyPort, reply);
                    }
                }
                else if(clientList[clientIndex].filenameComplete == 0)
                {
                    printf("E: Server (Filedata) (Serving Client %d): Filename transfer is not complete\n", replyPort);

                    init_clientData(&clientList[clientIndex]);
                    reply.arr[0] = NOT_OK;
                    Send(replyPort, reply);

                    //Send READY to a waiting list client
                    waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                    if(waitingClientReplyPort != -1)
                    {
                        printf("D: Server (Filedata) (Reviving Client %d)\n", waitingClientReplyPort);
                        reply.arr[0] = READY;
                        Send(waitingClientReplyPort, reply);
                    }
                }
                else if(dataLen + clientList[clientIndex].fileSize > MAX_FILESIZE)
                {
                    printf("E: Server (Filedata) (Serving Client %d): Maximum file size reached\n", replyPort);

                    fclose(clientList[clientIndex].fp);
                    init_clientData(&clientList[clientIndex]);
                    reply.arr[0] = NOT_OK;
                    Send(replyPort, reply);

                    //Send READY to a waiting list client
                    waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                    if(waitingClientReplyPort != -1)
                    {
                        printf("D: Server (Filedata) (Reviving Client %d)\n", waitingClientReplyPort);
                        reply.arr[0] = READY;
                        Send(waitingClientReplyPort, reply);
                    }
                }
                else
                {
                    // Append data to file
                    for(int i=0; i<dataLen; i++)
                    {
                        fputc(request.arr[3+i], clientList[clientIndex].fp);
                        clientList[clientIndex].fileSize++;
                    }
                    
                    //Reply OK
                    reply.arr[0] = OK;
                    Send(replyPort, reply);
                }
                break;

            case 2: 
            //==============End of File==============//
                if(clientIndex == -1)
                {
                    printf("E: Server (EOF) (Serving Client %d): Strange, no empty slots!\n", replyPort); 
                    printf("                              Client should have been allocated an element from clientList\n");

                    reply.arr[0] = NOT_OK;
                    Send(replyPort, reply);

                    //Send READY to a waiting list client
                    waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                    if(waitingClientReplyPort != -1)
                    {
                        printf("D: Server (EOF) (Reviving Client %d)\n", waitingClientReplyPort);
                        reply.arr[0] = READY;
                        Send(waitingClientReplyPort, reply);
                    }
                }
                else if(clientList[clientIndex].clientId == SERVER_PORT)
                {
                    printf("E: Server (EOF) (Serving Client %d): Client is directly sending filedata or Reply Port is incorrect\n", replyPort);

                    init_clientData(&clientList[clientIndex]);
                    reply.arr[0] = NOT_OK;
                    Send(replyPort, reply);
                
                    //Send READY to a waiting list client
                    waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                    if(waitingClientReplyPort != -1)
                    {
                        printf("D: Server (EOF) (Reviving Client %d)\n", waitingClientReplyPort);
                        reply.arr[0] = READY;
                        Send(waitingClientReplyPort, reply);
                    }
                }
                else if(clientList[clientIndex].filenameComplete == 0)
                {
                    printf("E: Server (EOF) (Serving Client %d): Filename transfer is not complete\n", replyPort);

                    init_clientData(&clientList[clientIndex]);
                    reply.arr[0] = NOT_OK;
                    Send(replyPort, reply);

                    //Send READY to a waiting list client
                    waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                    if(waitingClientReplyPort != -1)
                    {
                        printf("D: Server (EOF) (Reviving Client %d)\n", waitingClientReplyPort);
                        reply.arr[0] = READY;
                        Send(waitingClientReplyPort, reply);
                    }
                }
                else
                {
                    fclose(clientList[clientIndex].fp);
                    init_clientData(&clientList[clientIndex]);
                    reply.arr[0] = OK;
                    Send(replyPort, reply);

                    //Send READY to a waiting list client
                    waitingClientReplyPort = popQ(waitingClientQ, WAITING_QUEUE_LEN, &inQ, &outQ);
                    if(waitingClientReplyPort != -1)
                    {
                        printf("D: Server (EOF) (Reviving Client %d)\n", waitingClientReplyPort);
                        reply.arr[0] = READY;
                        Send(waitingClientReplyPort, reply);
                    }

                }
                break;

            default:
            //==============Invalid Request==============//
                
                printf("E: Server(Serving Client %d): Invalid Request Received\n", replyPort);

                reply.arr[0] = NOT_OK;
                Send(replyPort, reply);
                break;
        }
    }

    //=======Terminal State=======/
    // Not really needed by the server
    while(1)
    {
        //printf("Server\n");
        yield();
    }
}

int main(int argc, int **argv)
{
    int nClients = atoi((const char*)argv[1]);

    if(nClients != argc-2)
    {
        printf("Pass proper number of arguments\n"); //TODO: Make the message more informative
        return 1;
    }

    //Initialize all ports
    for(int iter = 0; iter < 100; iter++)
    {
        initPort(&(ports[iter]));
    }

    //Initialize ReadyQ
    ReadyQ = NewQueue();

    //Starting Server Thread
    start_thread(Server, SERVER_PORT, 0, "NULL");
    printf("Server Created\n");

    //Starting Client Threads
    for(int clientId = 1; clientId <= nClients; clientId++)
    {
        start_thread(Client, clientId, SERVER_PORT, (char*)argv[clientId+1]);
        printf("Client %d Created\n", clientId);
    }

    run();

    return 0;
}
