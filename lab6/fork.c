#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
  if (fork() < 0)
  {
    printf("fork failed\n");
    return 1;
  }
  return 0;
}
