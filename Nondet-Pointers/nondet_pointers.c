void nondet_single()
{
  int x = 42;
 
  int *p;
  __CPROVER_assume(p == &x);

  int z = *p;
  assert(42 <= z && z <= 43);
}

void nondet_multiple_disjunction()
{
  int x = 42;
  int y = 43;
 
  int *p;
  __CPROVER_assume(p == &x || p == &y);

  int z = *p;
  assert(42 <= z && z <= 43); // ?
}

void nondet_multiple_choice()
{
  int x = 42;
  int y = 43;
 
  int *p;
  int c;
  if (c)
    __CPROVER_assume(p == &x);
  else
    __CPROVER_assume(p == &y);

  int z = *p;
  assert(42 <= z && z <= 43);
}

void main()
{
  nondet_single();
  nondet_multiple_disjunction();
  nondet_multiple_choice();
}
