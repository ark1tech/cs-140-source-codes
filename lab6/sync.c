#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#define NTHREADS 4
#define EACH 1000000

uint64_t sum = 0;

void add()
{
  for (int i = 0; i < EACH; i++)
  {
    sum += 1;
  }
}

int main()
{
  pthread_t thread[NTHREADS];
  for (int i = 0; i < NTHREADS; i++)
  {
    pthread_create(&thread[i], NULL, (void *)add, NULL);
  }
  for (int i = 0; i < NTHREADS; i++)
  {
    pthread_join(thread[i], NULL);
  }
  printf("Sum: %lu\n", sum);
  return 0;
}