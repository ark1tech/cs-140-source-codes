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

  // add delay so extra threads arrive after first batch exits
  sleep(1);
}

void extra_f(void *id)
{
  for (int i = 0; i < 100000; i++)
  {
  }
  printf("\n*** EXTRA thread %d trying to use barrier ***\n", *((int *)id));
  wait_barrier(&b, NTHREADS);
  printf("*** EXTRA thread %d exited barrier ***\n", *((int *)id));
}

int main()
{
  pthread_t thread[NTHREADS + EXTRA_THREADS];
  int ids[NTHREADS + EXTRA_THREADS];

  printf("initializing barrier for N=%d threads\n", NTHREADS);
  printf("initial: count=0, mutex=1, barrier=0\n\n");
  init_barrier(&b);

  // create N threads
  for (int i = 0; i < NTHREADS; i++)
  {
    ids[i] = i;
    pthread_create(&thread[i], NULL, (void *)f, &ids[i]);
  }

  // wait a bit then create extra threads
  sleep(1);
  printf("\n*** creating %d EXTRA threads (total > N) ===\n", EXTRA_THREADS);
  for (int i = NTHREADS; i < NTHREADS + EXTRA_THREADS; i++)
  {
    ids[i] = i;
    pthread_create(&thread[i], NULL, (void *)extra_f, &ids[i]);
  }

  for (int i = 0; i < NTHREADS + EXTRA_THREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }

  printf("\nall threads completed successfully\n");
  printf("barrier ensured only N threads pass at a time\n");
}
