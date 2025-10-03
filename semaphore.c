#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 8

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
  sem_wait(&b->mutex);
  b->count += 1;
  sem_post(&b->mutex);
  if (b->count == n)
  {
    sem_post(&b->barrier);
  }
  sem_wait(&b->barrier);
  sem_post(&b->barrier); // (A)
}

void f(void *id)
{
  for (int i = 0; i < 1000000; i++)
  {
  }
  printf("Thread %d arrived at the barrier\n", *((int *)id));
  wait_barrier(&b, NTHREADS);
  printf("Thread %d exited the barrier\n", *((int *)id));
}

int main()
{
  pthread_t thread[NTHREADS];
  int ids[NTHREADS];
  init_barrier(&b);
  for (int i = 0; i < NTHREADS; i++)
  {
    ids[i] = i;
    pthread_create(&thread[i], NULL, (void *)f, &ids[i]);
  }
  for (int i = 0; i < NTHREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }
}