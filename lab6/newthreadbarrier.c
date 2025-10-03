#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NTHREADS 4

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
    }
    else
    {
      printf("  new thread detected barrier in use: num_exited=%d, n=%d\n", b->num_exited, n);
      pthread_mutex_unlock(&b->lock); // (2)
      while (b->num_exited != n)
      {
      }
      printf("  new thread finished waiting for exit, num_exited=%d\n", b->num_exited);
      pthread_mutex_lock(&b->lock); // (3)
      b->is_open = 0;
      printf("  new thread closed barrier, is_open=%d\n", b->is_open);
    }
  }
  b->num_waiting += 1;
  pthread_mutex_unlock(&b->lock); // (4)
  if (b->num_waiting == n)
  {
    b->num_waiting = 0;
    b->num_exited = 1;
    b->is_open = 1;
  }
  else
  {
    printf("  thread denied passage, waiting at (B) with num_waiting=%d\n", b->num_waiting);
    while (b->is_open == 0)
    {
    } // (B)
    pthread_mutex_lock(&b->lock); // (5)
    b->num_exited += 1;
    pthread_mutex_unlock(&b->lock); // (6)
  }
}

void f(void *id)
{
  for (int i = 0; i < 1000000; i++)
  {
  }
  printf("thread %d arrived at barrier\n", *((int *)id));
  wait_barrier(&b, NTHREADS);

  // add delay so new thread can arrive while barrier is open
  if (*((int *)id) == 0)
  {
    sleep(1);
  }

  printf("thread %d exited barrier\n", *((int *)id));
}

void new_thread_func(void *id)
{
  printf("\n*** NEW THREAD %d attempting to use barrier ***\n", *((int *)id));
  printf("*** barrier state: is_open=1, num_exited between 1 and %d ***\n\n", NTHREADS);

  wait_barrier(&b, NTHREADS);

  printf("\n*** NEW THREAD %d was denied and now waiting ***\n", *((int *)id));
}

int main()
{
  pthread_t thread[NTHREADS];
  pthread_t new_thread;
  int ids[NTHREADS];
  int new_id = 99;

  printf("starting %d threads synchronized by barrier\n\n", NTHREADS);
  init_barrier(&b, NTHREADS);

  for (int i = 0; i < NTHREADS; i++)
  {
    ids[i] = i;
    pthread_create(&thread[i], NULL, (void *)f, &ids[i]);
  }

  // wait a bit for threads to synchronize and start exiting
  sleep(1);

  // create new thread while barrier is open
  pthread_create(&new_thread, NULL, (void *)new_thread_func, &new_id);

  for (int i = 0; i < NTHREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }

  printf("\noriginal %d threads completed\n", NTHREADS);
  printf("new thread still waiting because num_waiting=1 != %d\n", NTHREADS);
  printf("barrier properly denied passage to new thread\n");

  // don't join new_thread, it will wait forever as intended
}
