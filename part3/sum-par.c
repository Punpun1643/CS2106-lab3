#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "config.h"
#include "barrier.h"

#define NUM_PROCESSES  8

int main() {

    int vect[VECT_SIZE];
    int pid;
    int shmid1;
    long int *all_sum;
    clock_t start, end;
    double time_taken;

    float per_process_raw = (float) VECT_SIZE / NUM_PROCESSES;
    int per_process = (int) per_process_raw;

    if(per_process_raw != (float) per_process) {
        printf("Vector size of %d is not divisible by %d processes.\n", VECT_SIZE, NUM_PROCESSES);
        exit(-1);
    }

    int i;

    srand(24601);
    for(i=0; i<VECT_SIZE; i++)
        vect[i] = rand();

    shmid1 = shmget(IPC_PRIVATE, NUM_PROCESSES * sizeof(int), IPC_CREAT | 0600);
    all_sum = (long int *) shmat(shmid1, NULL, 0);
    
    init_barrier(NUM_PROCESSES+1);

    for(i=0; i<NUM_PROCESSES; i++) {
        pid = fork();

        if(pid == 0)
            break;
    }

    int j;
    long int sum = 0;

    if(pid == 0) {
     // Calculate the correct sum of the process's portion of the vector
     int start_index = i * per_process;
     int end_index = start_index + per_process;
     
     for (int j=start_index; j < end_index; j++) {
        sum += vect[j];
     }
     
     all_sum[i] = sum;
     reach_barrier();
     
     exit(0);
    }
    else 
    {
        start = clock();
        reach_barrier();
        for (int i=0; i < NUM_PROCESSES; i++) {
          wait(NULL);
        }
        
        for (int i = 0; i < NUM_PROCESSES; i++) {
          sum += all_sum[i];
        }
	      end = clock();

        time_taken = ((double) end - start) / CLOCKS_PER_SEC;

        printf("\nNumber of items: %d\n", VECT_SIZE);
        printf("Sum element is %ld\n", sum);
        printf("Time taken is %3.10f\n\n", time_taken);

        // Clean up process table
        for(j=0; j<NUM_PROCESSES; j++)
            wait(NULL);

        shmdt(all_sum);
        shmctl(shmid1, IPC_RMID, 0);
        destroy_barrier(pid);
     }
}

