#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

#define NUM_CHILDREN 5

int main()
{
    // Create a shared memory segment
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    if (shmid < 0) {
      perror("Cannot create shared memory!\n");
      exit(1);
    }

    int i;
    
    // Attach shared memory segment to counter
    int *counter = (int *) shmat(shmid, NULL, 0);
    if (counter == (void *)-1) {
      perror("Cannot attach memory!\n");
      exit(1);
    }
    
    // Initialize counter
    *counter = 0;
    pid_t pid;
    
    // Initialize turn
    int *turn;
    int shmid_turn;
    shmid_turn = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    turn = (int *) shmat(shmid_turn, NULL, 0);
    *turn = 0;

    // Initialize semaphore
    sem_t *sem_arr;
    int shmid_semaphore_arr;

    shmid_semaphore_arr = shmget(IPC_PRIVATE, sizeof(sem_t) * NUM_CHILDREN, IPC_CREAT | 0600);
    sem_arr = (sem_t *) shmat(shmid_semaphore_arr, NULL, 0);

    // Initialize the value in the semaphore arrays
    for (int i=0; i < NUM_CHILDREN; i++) {
      if (i == 0) {
        // Set the first semaphore value to 1
        sem_init(&sem_arr[i], 1, 1);
      } else {
        sem_init(&sem_arr[i], 1, 0);  
      }
    }
    
    for (i = 0; i < NUM_CHILDREN; i++)
    {
        pid = fork();
        if (pid == 0)
            break;
    }
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        sem_wait(&sem_arr[i]);

        // Child process
        printf("Child %d starts\n", i + 1);
        // Simulate some work
        for (int j = 0; j < 5; j++)
        {
            (*counter)++;
            printf("Child %d increment counter %d\n", i + 1, *counter);
            fflush(stdout);
            usleep(250000);
        }
        printf("Child %d finishes with counter %d\n", i + 1, *counter);
        if (i != NUM_CHILDREN-1) {
          // Set the next semaphore to 1
          sem_post(&sem_arr[i+1]);
        }

        // Processing done, increment turn to next process
        *turn = *turn + 1;

        exit(EXIT_SUCCESS);
    }

    // Parent process
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        wait(NULL);
    }

    // Detach turn 
    shmdt((char *) turn);
    shmctl(shmid_turn, IPC_RMID, 0);

    // Print the final value of the counter
    printf("Final counter value: %d\n", *counter);
    
    // Detach and destroy shared memory segment
    shmdt((char *) counter);
    shmctl(shmid, IPC_RMID, 0);

    // Detach and destroy semaphores
    shmdt((char *) sem_arr);
    shmctl(shmid_semaphore_arr, IPC_RMID, 0);

    return 0;
}
