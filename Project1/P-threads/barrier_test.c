#include <stdio.h>

#include "barrier.h"

Barrier_t B;

void function(char *ch)
{
    printf("%s Here\n", ch);
    barrier(&B);
}

int main()
{
    barrier_init(&B, 4);

    start_thread(function, "Arth");
    start_thread(function, "B");
    start_thread(function, "C");

    barrier(&B);

    printf("All threads finished their work, or really?!\n");
    while(1);
    return 0;
}
