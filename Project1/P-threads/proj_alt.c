// Author: Arth Bhatt
// ASU-ID: 1215361875
// Project 1 Source Code

#include <stdio.h>
#include "threads.h"

int array[3] = {0,0,0};
int done[3] = {0,0,0};
int printing = 0;

int done_count = 0;

void child(int id)
{
    int cg;

    while(1)
    {
	while(printing == 1)
	    printf("Child %d : printing = %d\n", id, printing);
	//if(done[id]==0)
	
	{
	    array[id]++;
	    done[id] = 1;
	    done_count++;
	    printf("Child %d done. Enter an integer:\n", id);

	    while(printing == 0);
	    while(printing == 1);
	    //scanf("%d", &cg);
	    //pthread_yield();
	}
    }
}

int main()
{
    int cg;
    start_thread(child, 0);
    start_thread(child, 1);
    start_thread(child, 2);

    while(1)
    {
	//while((done[0] == 0) || (done[1]== 0) || (done[2]==0) ) 
	while(done_count >= 3)
	{
	    printf("done[0] = %d done[1] = %d done[2] = %d\n", done[0], done[1], done[2]);
	    printf("printing = %d\n", printing);
	}
    
	printing = 1;
	if(printing == 1)
	{

	    printf("PRINTING : array = {%d, %d, %d}\n", array[0], \
						        array[1], \
						        array[2]);
	    done[0] = 0;
	    done[1] = 0;
	    done[2] = 0;

	    done_count = 0;
	    printing = 0;
	    //printf("Printing done. Enter an integer:");
	    //scanf("%d", &cg);
	    //pthread_yield();

	    sleep(1); 
	}
    
    }
    return 0;
}
