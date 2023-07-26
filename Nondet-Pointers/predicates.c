#include <assert.h>
#include <stdlib.h>

void main()
{
  int *null = 0;
  int *nondet;
  const char *constant = "bla";
  char stack[5];
  char *heap = malloc(5 * sizeof(char));
  char *invalid = (size_t)1 << (sizeof(char *) * 8 - 8);

  assert(!__CPROVER_is_invalid_pointer(null));
  assert(__CPROVER_is_invalid_pointer(nondet));
  assert(!__CPROVER_is_invalid_pointer(nondet));
  assert(!__CPROVER_is_invalid_pointer(constant));
  assert(!__CPROVER_is_invalid_pointer(stack));
  assert(!__CPROVER_is_invalid_pointer(heap));
  assert(__CPROVER_is_invalid_pointer(invalid));

  assert(!__CPROVER_DYNAMIC_OBJECT(null));
  assert(__CPROVER_DYNAMIC_OBJECT(nondet));
  assert(!__CPROVER_DYNAMIC_OBJECT(nondet));
  assert(!__CPROVER_DYNAMIC_OBJECT(constant));
  assert(!__CPROVER_DYNAMIC_OBJECT(stack));
  assert(__CPROVER_DYNAMIC_OBJECT(heap));
  assert(!__CPROVER_DYNAMIC_OBJECT(invalid));

  assert(!__CPROVER_r_ok(null));
  assert(__CPROVER_r_ok(nondet));
  assert(!__CPROVER_r_ok(nondet));
  assert(__CPROVER_r_ok(constant));
  assert(__CPROVER_r_ok(stack, 5));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(invalid));

  assert(!__CPROVER_w_ok(null));
  assert(__CPROVER_w_ok(nondet));
  assert(!__CPROVER_w_ok(nondet));
  assert(__CPROVER_w_ok(constant)); // ?
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_w_ok(invalid));
  
  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet));
  assert(!__CPROVER_rw_ok(nondet));
  assert(__CPROVER_rw_ok(constant)); // ?
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));

  free(heap);
  assert(!__CPROVER_is_invalid_pointer(heap));
  assert(__CPROVER_DYNAMIC_OBJECT(heap)); // ?
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(heap));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_w_ok(heap));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(heap));
}
