#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 10000

int global = 0;

void increment()
{
  while (global < N)
  {
    global++;
    printf("inc: %d\n", global);
  }
}
void decrement()
{
  while (global > -N)
  {
    global--;
    printf("dec: %d\n", global);
  }
}
int main()
{
  pthread_t t1, t2;
  pthread_create(&t1, NULL, (void *)increment, NULL);
  pthread_create(&t2, NULL, (void *)decrement, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  return 0;
}