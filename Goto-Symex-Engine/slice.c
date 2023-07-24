void main()
{
  int a[10];
  int b[10];
  int c[10];

  int i;
  for(i=0; i<10; ++i)
    b[i] = 2 * a[i];

  assert(2 * a[0] == b[0]);

  for(i=0; i<10; ++i)
    c[i] = b[i] - a[i];

  assert(a[0] == c[0]);
}
