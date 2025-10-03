#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NTHREADS 3

typedef struct barrier
{
  int count;
  sem_t mutex;
  sem_t barrier;
} Barrier;

Barrier b;

void init_barrier(Barrier *b)
{
  b->count = 0;
  sem_init(&b->mutex, 0, 1);   // Semaphore integer initialized to 1
  sem_init(&b->barrier, 0, 0); // Semaphore integer initialized to 0
}

void wait_barrier(Barrier *b, int n)
{
  int sem_value;

  sem_wait(&b->mutex);
  b->count += 1;
  printf("  thread incremented count to %d\n", b->count);
  sem_post(&b->mutex);

  if (b->count == n)
  {
    printf("  last thread (count=%d) posting to barrier\n", b->count);
    sem_post(&b->barrier);
    sem_getvalue(&b->barrier, &sem_value);
    printf("  barrier semaphore value: %d\n", sem_value);
  }

  sem_getvalue(&b->barrier, &sem_value);
  printf("  thread calling sem_wait (barrier value: %d)\n", sem_value);
  sem_wait(&b->barrier);
  printf("  thread passed sem_wait\n");

  // (A) COMMENTED OUT - THIS CAUSES DEADLOCK
  // sem_post(&b->barrier);

  sem_getvalue(&b->barrier, &sem_value);
  printf("  barrier value after (A) skipped: %d\n", sem_value);
}

void f(void *id)
{
  for (int i = 0; i < 500000; i++)
  {
  }
  printf("\nthread %d arrived at barrier\n", *((int *)id));
  wait_barrier(&b, NTHREADS);
  printf("thread %d exited barrier\n", *((int *)id));
}

int main()
{
  pthread_t thread[NTHREADS];
  int ids[NTHREADS];

  printf("demonstrating deadlock with line (A) commented out\n");
  printf("initial state: count=0, mutex=1, barrier=0\n");
  printf("N=%d threads\n\n", NTHREADS);

  init_barrier(&b);

  for (int i = 0; i < NTHREADS; i++)
  {
    ids[i] = i;
    pthread_create(&thread[i], NULL, (void *)f, &ids[i]);
  }

  printf("\nwaiting 3 seconds for threads to complete...\n");
  sleep(3);

  printf("\ndeadlock scenario! \n");
  printf("expected: all %d threads should exit\n", NTHREADS);
  printf("actual: only 1 thread exits, others deadlock at sem_wait\n\n");

  printf("explanation:\n");
  printf("- thread 0,1,2 arrive, count becomes 3\n");
  printf("- thread 2 (last) does sem_post, barrier value: 0 -> 1\n");
  printf("- thread 0 does sem_wait first, barrier: 1 -> 0, thread 0 exits\n");
  printf("- WITHOUT line (A), thread 0 doesn't do sem_post\n");
  printf("- barrier stays at 0\n");
  printf("- threads 1,2 wait forever at sem_wait (barrier=0)\n");
  printf("- DEADLOCK: threads 1,2 never wake up\n\n");

  printf("the sem_post at (A) is needed so each exiting thread\n");
  printf("posts to wake the next thread in a chain reaction\n");

  // note: threads will hang, program won't exit cleanly
  // this demonstrates the deadlock

  for (int i = 0; i < NTHREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }

  printf("\nif you see this, all threads completed (shouldn't happen with deadlock)\n");
}
