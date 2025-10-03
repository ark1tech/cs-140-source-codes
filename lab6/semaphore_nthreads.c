#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NTHREADS 4
#define EXTRA_THREADS 2

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
    printf("  last thread (count=%d) posting to barrier semaphore\n", b->count);
    sem_post(&b->barrier);
    sem_getvalue(&b->barrier, &sem_value);
    printf("  barrier semaphore value after post: %d\n", sem_value);
  }

  sem_getvalue(&b->barrier, &sem_value);
  printf("  thread calling sem_wait on barrier (current value: %d)\n", sem_value);
  sem_wait(&b->barrier);
  printf("  thread passed sem_wait on barrier\n");

  sem_post(&b->barrier); // (A)
  sem_getvalue(&b->barrier, &sem_value);
  printf("  thread posted to barrier (new value: %d)\n", sem_value);
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
  pthread_t thread[NTHREADS + EXTRA_THREADS];
  int ids[NTHREADS + EXTRA_THREADS];

  printf("initializing barrier for N=%d threads\n", NTHREADS);
  printf("creating %d threads total (N + %d extra)\n", NTHREADS + EXTRA_THREADS, EXTRA_THREADS);
  printf("initial: count=0, mutex=1, barrier=0\n\n");
  init_barrier(&b);

  // create all threads at the same time (N + extra)
  for (int i = 0; i < NTHREADS + EXTRA_THREADS; i++)
  {
    ids[i] = i;
    pthread_create(&thread[i], NULL, (void *)f, &ids[i]);
  }

  for (int i = 0; i < NTHREADS + EXTRA_THREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }

  printf("\nall %d threads completed\n", NTHREADS + EXTRA_THREADS);
  printf("first N=%d threads passed through barrier together\n", NTHREADS);
  printf("remaining %d threads waited and formed new barrier cycle\n", EXTRA_THREADS);
}
