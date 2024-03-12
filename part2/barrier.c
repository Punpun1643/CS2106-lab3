#include <stdio.h>

void init_barrier(int numproc) {
}

void reach_barrier() {
}

void destroy_barrier(int my_pid) {
    if(my_pid != 0) {
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.
    }
}


