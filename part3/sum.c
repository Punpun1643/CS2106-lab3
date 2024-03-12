#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "config.h"

// Sort through an array of VECT_SIZE integers

int main() {
    int vect[VECT_SIZE], i;
    clock_t start, end;
    double time_taken;

    srand(24601);
    for(i=0; i<VECT_SIZE; i++) {
        vect[i] = rand();
    }

    long int sum=0;
   
    start = clock();
    for(i=0; i<VECT_SIZE; i++) {
        sum+=vect[i];

      
    }
    end = clock();
    time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nNumber of items: %d\n", VECT_SIZE);
    printf("Sum %ld\n", sum);
     printf("Time taken is %3.10f\n\n", time_taken);
}

