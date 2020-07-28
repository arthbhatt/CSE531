// Authors: Arth Bhatt and Akhilesh Reddy Eppa
// ASU-ID: 1215361875 and 1217207641
// Project 1 Source Code

#include <stdio.h>
#include "sem.h"

semaphore_t mutex;
int array[3] = {0,0,0};
int done[3] = {0,0,0};
int printing=0, updating=0;

void child(int id)
{
    while(1)
    {
	P(&mutex);
	
	if(printing == 0) // This condition is not needed
	{
	   updating = 1;
	}

	if(done[id] == 0)
	{
	    V(&mutex);
	
	    printf("UPDATING : array[%d] = %d\n", id, array[id]);
	    array[id]++;
	
	    P(&mutex);
	}
	
	if( (done[(id+1)%3 ] == 1) && (done[(id+2)%3 ] == 1) )
	{
	    done[id] = 1;
	    updating = 0;
	}
	else 
	    done[id] = 1;
	
	V(&mutex);
    }
}

int main()
{
    init_sem(&mutex, 1);

    start_thread(child, 0);
    start_thread(child, 1);
    start_thread(child, 2);

    while(1)
    {
	P(&mutex);

	if(updating == 0)
	{
	    printing = 1;

	    printf("PRINTING : array = {%d, %d, %d}\n\n", array[0], \
						        array[1], \
						        array[2]);
	    done[0] = 0;
	    done[1] = 0;
	    done[2] = 0;

	    printing = 0;
	}
	
        V(&mutex);
    }
    return 0;
}
