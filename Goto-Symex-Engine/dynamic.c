struct list
{
  int x;
  struct list *next;
};

void main()
{
  struct list *p = 0;
  for (int i=0; i<5; ++i)
  {
    int c;
    if (c) break;
    struct list *n = malloc(sizeof(struct list));
    n->next = p;
    n->x = i;
    p = n;
  }
  
  assert(!(p == 0));
  assert(!(p != 0));
  
  __CPROVER_assume(p != 0);
  int x = p->x;
  assert(!(x == 0));
  assert(!(x == 1));
  assert(!(x == 2));
  assert(!(x == 3));
  assert(!(x == 4));
  assert(!(x == 5));
}
