// maximum 14
#define NUMBER_OF_MULT 14

void main()
{
  long long x;
  __CPROVER_assume(1ll <= x && x < 16ll);

  long long z = 1;
  for (int i=0; i<NUMBER_OF_MULT; ++i)
    z *= x;

  assert(1ll <= z && z < 16ll << (NUMBER_OF_MULT * 4));
}
