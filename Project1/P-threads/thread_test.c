// (c) Partha Dasgupta 2009
// permission to use and distribute granted.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int global=0;

void function_1(int *arg)
{
    int local = 0;
    while (1){
        printf("%d: XXXXPrinting from Function 1 [arg = %d] global = %d  local = %d\n", pthread_self(), *arg, global, local);
        sleep(1);
        global++; local ++;
        printf("%d: Function 1 incremented .... global = %d  local = %d\n", pthread_self(), global, local);
        sleep(1);
    }
}    

void function_2(int *arg)
{
   int local = 0;

    while (1){
        printf("XXXXPrinting from Function 2 [arg = %d] global = %d  local = %d\n", *arg, global, local);
        sleep(1);
        global++; local ++;
        printf("Function 2 incremented .... global = %d  local = %d\n", global, local);
        sleep(1);
    }
}    

void function_3(int *arg)
{
   int local = 0;

    while (1){
        printf("XXXXPrinting from Function 3 [arg = %d] global = %d  local = %d\n", *arg, global, local);
        sleep(1);
        global++; local ++;
        printf("Function 3 incremented .... global = %d  local = %d\n", global, local);
        sleep(1);
    }
}    

pthread_t start_thread(void *func, int *arg)
  {
   pthread_t thread_id;
   int rc;
      printf("In main: creating thread\n");
      rc = pthread_create(&thread_id, NULL, func, arg);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
        }
    return(thread_id);
   }


int main()
{  int arg1, arg2, arg3;

    arg1 = 25; start_thread(function_1, &arg1);
    arg2 = 50; start_thread(function_2, &arg2); // there is a serious bug here, left it in for demo purposes
    arg3 = 100; start_thread(function_3, &arg3);

    while(1) sleep(1); // infinite loop

    return 0;
}




