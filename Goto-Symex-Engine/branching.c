void main()
{
  int x;
  __CPROVER_assume(-1000 <= x && x <= 1000);
  
  int c;
  if (c == 0)
    x = x < 0 ? -x : x;
  else
    x *= x;

  assert(x >= 0);
}
