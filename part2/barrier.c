#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

sem_t *barrier;
int shmid_barrier;

int shmid_count;
int *count;

int nproc;

sem_t *mutex;

void init_barrier(int numproc) {
  nproc = numproc;

  // Initialize count
  shmid_count = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
  count = (int *) shmat(shmid_count, NULL, 0);
  *count = 0;

  // Initialize barrier
  shmid_barrier = shmget(IPC_PRIVATE, sizeof(sem_t) * 2, IPC_CREAT | 0600);
  barrier = (sem_t*) shmat(shmid_barrier, NULL, 0);

  // Initialize semaphores for selecting process
  sem_init(barrier, 1, 0);

  // Initialize mutex
  mutex = &barrier[1];
  sem_init(mutex, 1, 1);
}

void reach_barrier() {
  sem_wait(mutex);
  *count = *count + 1;

  if (*count == nproc) { // last process
    sem_post(mutex);
    sem_post(barrier);
  } else { // not last process
    sem_post(mutex);
    sem_wait(barrier);
    sem_post(barrier);
  }
}

void destroy_barrier(int my_pid) {
    if(my_pid != 0) {
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.
        shmdt(barrier);
        shmdt(count);
        shmctl(shmid_barrier, IPC_RMID, 0);
        shmctl(shmid_count, IPC_RMID, 0);
    }
}

