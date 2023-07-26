#include <assert.h>
#include <stdlib.h>

void assume_r_ok()
{
  int *null = 0;
  int *nondet;
  const char *constant = "bla";
  char stack[5];
  char *heap = malloc(5 * sizeof(char));
  char *invalid = (size_t)1 << (sizeof(char *) * 8 - 8);

  assert(!__CPROVER_r_ok(null));
  assert(__CPROVER_r_ok(nondet));
  assert(!__CPROVER_r_ok(nondet));
  assert(__CPROVER_r_ok(constant));
  assert(__CPROVER_r_ok(stack, 5));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(invalid));
  
  __CPROVER_assume(__CPROVER_r_ok(null));
  __CPROVER_assume(__CPROVER_r_ok(nondet));
  __CPROVER_assume(__CPROVER_r_ok(constant));
  __CPROVER_assume(__CPROVER_r_ok(stack, 5));
  __CPROVER_assume(__CPROVER_r_ok(heap));
  __CPROVER_assume(__CPROVER_r_ok(invalid));

  assert(!__CPROVER_r_ok(null));
  assert(__CPROVER_r_ok(nondet)); // ?
  assert(!__CPROVER_r_ok(nondet)); // ?
  assert(__CPROVER_r_ok(constant));
  assert(__CPROVER_r_ok(stack, 5));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(invalid));

  assert(!__CPROVER_w_ok(null));
  assert(__CPROVER_w_ok(nondet)); // ?
  assert(!__CPROVER_w_ok(nondet)); // ?
  assert(__CPROVER_w_ok(constant)); // ?
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_r_ok(invalid));
  
  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet)); // ?
  assert(!__CPROVER_rw_ok(nondet)); // ?
  assert(__CPROVER_rw_ok(constant)); // ?
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));

}

void assume_not_r_ok()
{
  int *null = 0;
  int *nondet;
  const char *constant = "bla";
  char stack[5];
  char *heap = malloc(5 * sizeof(char));
  char *invalid = (size_t)1 << (sizeof(char *) * 8 - 8);

  assert(!__CPROVER_r_ok(null));
  assert(__CPROVER_r_ok(nondet)); // ?
  assert(!__CPROVER_r_ok(nondet)); // ?
  assert(__CPROVER_r_ok(constant));
  assert(__CPROVER_r_ok(stack, 5));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(invalid));
  
  __CPROVER_assume(!__CPROVER_r_ok(null));
  __CPROVER_assume(!__CPROVER_r_ok(nondet));
  __CPROVER_assume(!__CPROVER_r_ok(constant));
  __CPROVER_assume(!__CPROVER_r_ok(stack, 5));
  __CPROVER_assume(!__CPROVER_r_ok(heap));
  __CPROVER_assume(!__CPROVER_r_ok(invalid));

  assert(!__CPROVER_r_ok(null));
  assert(__CPROVER_r_ok(nondet)); // ?
  assert(!__CPROVER_r_ok(nondet)); // ?
  assert(__CPROVER_r_ok(constant));
  assert(__CPROVER_r_ok(stack, 5));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(invalid));

  assert(!__CPROVER_w_ok(null));
  assert(__CPROVER_w_ok(nondet)); // ?
  assert(!__CPROVER_w_ok(nondet)); // ?
  assert(__CPROVER_w_ok(constant)); // ?
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_r_ok(invalid));
  
  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet)); // ?
  assert(!__CPROVER_rw_ok(nondet)); // ?
  assert(__CPROVER_rw_ok(constant)); // ?
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));
}

void assume_w_ok()
{
  int *null = 0;
  int *nondet;
  const char *constant = "bla";
  char stack[5];
  char *heap = malloc(5 * sizeof(char));
  char *invalid = (size_t)1 << (sizeof(char *) * 8 - 8);

  assert(!__CPROVER_w_ok(null));
  assert(__CPROVER_w_ok(nondet)); // ?
  assert(!__CPROVER_w_ok(nondet)); // ?
  assert(__CPROVER_w_ok(constant)); // ?
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_r_ok(invalid));
  
  __CPROVER_assume(__CPROVER_w_ok(null));
  __CPROVER_assume(__CPROVER_w_ok(nondet));
  __CPROVER_assume(__CPROVER_w_ok(constant));
  __CPROVER_assume(__CPROVER_w_ok(stack, 5));
  __CPROVER_assume(__CPROVER_w_ok(heap));
  __CPROVER_assume(__CPROVER_w_ok(invalid));

  assert(!__CPROVER_r_ok(null));
  assert(__CPROVER_r_ok(nondet)); // ?
  assert(!__CPROVER_r_ok(nondet)); // ?
  assert(__CPROVER_r_ok(constant));
  assert(__CPROVER_r_ok(stack, 5));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(invalid));

  assert(!__CPROVER_w_ok(null));
  assert(__CPROVER_w_ok(nondet)); // ?
  assert(!__CPROVER_w_ok(nondet)); // ?
  assert(__CPROVER_w_ok(constant)); // ?
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_r_ok(invalid));
  
  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet)); // ?
  assert(!__CPROVER_rw_ok(nondet)); // ?
  assert(__CPROVER_rw_ok(constant)); // ?
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));

}

void assume_not_w_ok()
{
  int *null = 0;
  int *nondet;
  const char *constant = "bla";
  char stack[5];
  char *heap = malloc(5 * sizeof(char));
  char *invalid = (size_t)1 << (sizeof(char *) * 8 - 8);

  assert(!__CPROVER_w_ok(null));
  assert(__CPROVER_w_ok(nondet)); // ?
  assert(!__CPROVER_w_ok(nondet)); // ?
  assert(__CPROVER_w_ok(constant)); // ?
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_w_ok(invalid));
  
  __CPROVER_assume(!__CPROVER_w_ok(null));
  __CPROVER_assume(!__CPROVER_w_ok(nondet));
  __CPROVER_assume(!__CPROVER_w_ok(constant));
  __CPROVER_assume(!__CPROVER_w_ok(stack, 5));
  __CPROVER_assume(!__CPROVER_w_ok(heap));
  __CPROVER_assume(!__CPROVER_w_ok(invalid));

  assert(!__CPROVER_r_ok(null));
  assert(__CPROVER_r_ok(nondet)); // ?
  assert(!__CPROVER_r_ok(nondet)); // ?
  assert(__CPROVER_r_ok(constant));
  assert(__CPROVER_r_ok(stack, 5));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(invalid));

  assert(!__CPROVER_w_ok(null));
  assert(__CPROVER_w_ok(nondet)); // ?
  assert(!__CPROVER_w_ok(nondet)); // ?
  assert(__CPROVER_w_ok(constant)); // ?
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_w_ok(invalid));
  
  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet)); // ?
  assert(!__CPROVER_rw_ok(nondet)); // ?
  assert(__CPROVER_rw_ok(constant)); // ?
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));
}
void assume_rw_ok()
{
  int *null = 0;
  int *nondet;
  const char *constant = "bla";
  char stack[5];
  char *heap = malloc(5 * sizeof(char));
  char *invalid = (size_t)1 << (sizeof(char *) * 8 - 8);

  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet)); // ?
  assert(!__CPROVER_rw_ok(nondet)); // ?
  assert(__CPROVER_rw_ok(constant)); // ?
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));
  
  __CPROVER_assume(__CPROVER_rw_ok(null));
  __CPROVER_assume(__CPROVER_rw_ok(nondet));
  __CPROVER_assume(__CPROVER_rw_ok(constant));
  __CPROVER_assume(__CPROVER_rw_ok(stack, 5));
  __CPROVER_assume(__CPROVER_rw_ok(heap));
  __CPROVER_assume(__CPROVER_rw_ok(invalid));

  assert(!__CPROVER_r_ok(null));
  assert(__CPROVER_r_ok(nondet)); // ?
  assert(!__CPROVER_r_ok(nondet)); // ?
  assert(__CPROVER_r_ok(constant));
  assert(__CPROVER_r_ok(stack, 5));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(invalid));

  assert(!__CPROVER_w_ok(null));
  assert(__CPROVER_w_ok(nondet)); // ?
  assert(!__CPROVER_w_ok(nondet)); // ?
  assert(__CPROVER_w_ok(constant)); // ?
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_r_ok(invalid));
  
  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet)); // ?
  assert(!__CPROVER_rw_ok(nondet)); // ?
  assert(__CPROVER_rw_ok(constant)); // ?
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));

}

void assume_not_rw_ok()
{
  int *null = 0;
  int *nondet;
  const char *constant = "bla";
  char stack[5];
  char *heap = malloc(5 * sizeof(char));
  char *invalid = (size_t)1 << (sizeof(char *) * 8 - 8);

  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet));
  assert(!__CPROVER_rw_ok(nondet));
  assert(__CPROVER_rw_ok(constant)); // ?
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));
  
  __CPROVER_assume(!__CPROVER_rw_ok(null));
  __CPROVER_assume(!__CPROVER_rw_ok(nondet));
  __CPROVER_assume(!__CPROVER_rw_ok(constant));
  __CPROVER_assume(!__CPROVER_rw_ok(stack, 5));
  __CPROVER_assume(!__CPROVER_rw_ok(heap));
  __CPROVER_assume(!__CPROVER_rw_ok(invalid));

  assert(!__CPROVER_r_ok(null));
  assert(__CPROVER_r_ok(nondet)); // ?
  assert(!__CPROVER_r_ok(nondet)); // ?
  assert(__CPROVER_r_ok(constant));
  assert(__CPROVER_r_ok(stack, 5));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(invalid));

  assert(!__CPROVER_w_ok(null));
  assert(__CPROVER_w_ok(nondet)); // ?
  assert(!__CPROVER_w_ok(nondet)); // ?
  assert(__CPROVER_w_ok(constant)); // ?
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_w_ok(invalid));
  
  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet)); // ?
  assert(!__CPROVER_rw_ok(nondet)); // ?
  assert(__CPROVER_rw_ok(constant)); // ?
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));
}
void main()
{
  assume_r_ok();
  assume_not_r_ok();
  assume_w_ok();
  assume_not_w_ok();
  assume_rw_ok();
  assume_not_rw_ok();
}
