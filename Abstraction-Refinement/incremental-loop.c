#define BOUND 1242

extern int nondet();

void main()
{
  int x = nondet();
  __CPROVER_assume(0 <= x && x <= 1);
  while(1)
  {
    x = x + 1;
    assert(x < BOUND);
  }
}
