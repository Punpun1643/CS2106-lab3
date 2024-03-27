#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

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
        exit(EXIT_SUCCESS);
    }

    // Parent process
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        wait(NULL);
    }

    // Print the final value of thecounter
    printf("Final counter value: %d\n", *counter);
    
    // Detach and destroy shared memory segment
    shmdt((char *) counter);
    shmctl(shmid, IPC_RMID, 0);
    return 0;
}
