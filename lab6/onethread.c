#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *f(void *arg)
{
  printf("i am thread %lu\n", pthread_self());
  return NULL;
}

int main()
{
  pthread_t thread;
  int result;

  if (pthread_create(&thread, NULL, f, NULL) != 0)
  {
    printf("error creating thread\n");
    return 1;
  }

  pthread_join(thread, NULL);
  return 0;
}
