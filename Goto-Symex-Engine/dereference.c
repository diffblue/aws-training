void main()
{
  int x = 1;
  int y = 2;
  
  int *p;
  int c;
  if (c)
    p = &x;
  else
    p = &y;

  int z = *p;
  assert(c || z == 2);
}
