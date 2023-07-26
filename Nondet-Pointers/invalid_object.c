void main()
{
  int *p = 0;
  int x = *p;
  assert(x);
  assert(!x);

  *p = 1;
  x = *p;
  assert(x == 1);
  assert(x != 1);
}
