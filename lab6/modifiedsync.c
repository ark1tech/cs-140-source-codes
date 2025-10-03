#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#define NTHREADS 4
#define EACH 1000000

uint64_t sum = 0;
// mod
pthread_mutex_t mutex;

void add()
{
  for (int i = 0; i < EACH; i++)
  {
    // mod
    pthread_mutex_lock(&mutex);
    sum += 1;
    // mod
    pthread_mutex_unlock(&mutex);
  }
}

int main()
{
  pthread_t thread[NTHREADS];
  // mod
  pthread_mutex_init(&mutex, NULL);
  for (int i = 0; i < NTHREADS; i++)
  {
    pthread_create(&thread[i], NULL, (void *)add, NULL);
  }
  for (int i = 0; i < NTHREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }
  printf("Sum: %lu\n", sum);
  // mod
  pthread_mutex_destroy(&mutex);
  return 0;
}