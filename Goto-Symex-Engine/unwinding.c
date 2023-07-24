#include <pthread.h>

int power_iterative(int base, int exponent)
{
  int current = 1;
  for(int i=0; i<exponent; ++i)
    current *= base;
  return current;
}

int power_recursive(int base, int exponent, int current)
{
  if (exponent == 0)
    return current;
  return power_recursive(base, exponent - 1, current) * base;
}

int current = 1;

void worker(void *arg)
{
  int base = *(int *)arg;
  __CPROVER_atomic_begin();
  current *= base;
  __CPROVER_atomic_end();
}

int power_threaded(int base, int exponent)
{
  pthread_t thread[exponent];
  for(int i=0; i<exponent; ++i)
    pthread_create(&thread[i], NULL, worker, &base);
  for(int i=0; i<exponent; ++i)
    pthread_join(thread[i], NULL);
  return current;
}

void main()
{
  int base;
  __CPROVER_assume(0 <= base && base < 4);
  int exponent;
  __CPROVER_assume(0 <= exponent && exponent < 4);

  int iterative = power_iterative(base, exponent);
  int recursive = power_recursive(base, exponent, 1);
  int threaded = power_threaded(base, exponent);
  assert(iterative == recursive);
  assert(iterative == threaded);
}
