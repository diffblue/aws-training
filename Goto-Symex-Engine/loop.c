void main()
{
  int bound;
  __CPROVER_assume(3 <= bound && bound <= 3);
  int value;
  __CPROVER_assume(42 <= value && value <= 42);
  
  int x = 0;
  int y;
  while (x < bound)
  {
    ++x;
    y = value;
  }
  assert(y != value);
}
