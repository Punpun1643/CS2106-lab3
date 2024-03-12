#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDREN 5

int main() {

    int counter = 0,i;
    pid_t pid;

    for (i = 0; i < NUM_CHILDREN; i++) {
        pid = fork();
	if (pid==0) break;
  }
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            printf("Child %d starts\n", i + 1);
            // Simulate some work
            for (int j = 0; j < 5; j++) {
                counter++;       
		printf("Child %d increment counter %d\n", i + 1, counter);
		
            }
            printf("Child %d finishes with counter %d\n", i + 1, counter);
            exit(EXIT_SUCCESS);
        }
    
// Parent process
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    // Print the final value of thecounter
    printf("Final counter value: %d\n", counter);

    return 0;
}



