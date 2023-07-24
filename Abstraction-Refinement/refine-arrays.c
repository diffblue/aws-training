#define SIZE 1000

void main()
{
  int x[SIZE];
  int i;
  for (int i=0; i<SIZE; ++i)
    x[i] = 1;

  int j;
  __CPROVER_assume(0<=j && j<SIZE);
  assert(x[j] != 1);
}
