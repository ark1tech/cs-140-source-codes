#include <stdio.h>
#include <pthread.h>

#define NTHREADS 3

typedef struct barrier
{
  int num_waiting;
  int num_exited;
  int is_open;
  pthread_mutex_t lock;
} Barrier;

Barrier b;

void init_barrier(Barrier *b, int n)
{
  b->num_waiting = 0;
  b->num_exited = n;
  b->is_open = 0;
  pthread_mutex_init(&b->lock, NULL);
}

void wait_barrier(Barrier *b, int n)
{
  pthread_mutex_lock(&b->lock); // (1)
  if (b->num_waiting == 0)
  {
    if (b->num_exited == n)
    {                 // Hint: For reusability
      b->is_open = 0; // (A)
      printf("reached (A), num_exited=%d\n", b->num_exited);
    }
    else
    {
      pthread_mutex_unlock(&b->lock); // (2)
      while (b->num_exited != n)
      {
      }
      pthread_mutex_lock(&b->lock); // (3)
      b->is_open = 0;
    }
  }
  b->num_waiting += 1;
  printf("num_waiting now: %d\n", b->num_waiting);
  pthread_mutex_unlock(&b->lock); // (4)
  if (b->num_waiting == n)
  {
    printf("last thread, opening barrier\n");
    b->num_waiting = 0;
    b->num_exited = 1;
    b->is_open = 1;
  }
  else
  {
    printf("waiting at (B)\n");
    while (b->is_open == 0)
    {
    } // (B)
    printf("escaped (B)\n");
    pthread_mutex_lock(&b->lock); // (5)
    b->num_exited += 1;
    printf("num_exited now: %d\n", b->num_exited);
    pthread_mutex_unlock(&b->lock); // (6)
  }
}

void f(void *id)
{
  for (int i = 0; i < 1000000; i++)
  {
  }
  printf("\nthread %d arrived\n", *((int *)id));
  wait_barrier(&b, NTHREADS);
  printf("thread %d exited\n", *((int *)id));
}

int main()
{
  pthread_t thread[NTHREADS];
  int ids[NTHREADS];
  printf("starting with num_waiting=0, num_exited=3, is_open=0\n");
  init_barrier(&b, NTHREADS);
  for (int i = 0; i < NTHREADS; i++)
  {
    ids[i] = i;
    pthread_create(&thread[i], NULL, (void *)f, &ids[i]);
  }
  for (int i = 0; i < NTHREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }
  printf("\ndone\n");
}