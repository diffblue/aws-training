extern int nondet();

#define SIZE 8

void main()
{
  int x[SIZE];
  x[0] = 1;

  for(int i=1; i<SIZE; ++i)
  {
    x[i] = x[i-1];
  }

  assert(x[SIZE-1] != 1);
}
