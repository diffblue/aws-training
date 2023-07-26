void assume_r_ok()
{
  int *p;
  __CPROVER_assume(__CPROVER_r_ok(p));
  assert(p != 0);
  int x = *p;
  assert(x);
  assert(!x);
}


void assume_w_ok()
{
  int *p;
  __CPROVER_assume(__CPROVER_w_ok(p));
  assert(p != 0);
  *p = 1;
  int x = *p;
  assert(x == 1);
}

void assume_rw_ok()
{
  int *p;
  __CPROVER_assume(__CPROVER_rw_ok(p));
  assert(p != 0);
  *p = 1;
  int x = *p;
  assert(x == 1);
}

void main()
{
  assume_r_ok();
  assume_w_ok();
  assume_rw_ok();
}
