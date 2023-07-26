# Nondet-Pointers

## Introduction

This is a live tutorial on how CBMC deals with nondeterministic pointers.
We're going to look at:

1. CBMC's memory model
2. Pointer predicates
3. Auto-objects
4. Invalid objects
5. Assumptions on non-deterministic pointers

All of the tools that we are demoing today have been built out of `develop`
at the time when `cbmc-5.88.0` was released.

## CBMC's memory model

CBMC has an object-based memory model.
- Each memory allocation (local/global, stack/heap) is handled as an
  independent memory object.
- Entirely agnostic of stack layout and heap memory allocation
  strategies.
- Memory objects are identified symbolically. Sometimes identifiers of
  objects are encoded as "object numbers", but these aren't really addresses.
- Offset for bytes within objects.
- Null object

## Pointer predicates

```
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
  assert(__CPROVER_w_ok(constant)); 
  assert(__CPROVER_w_ok(stack, 5));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_w_ok(invalid));
  
  assert(!__CPROVER_rw_ok(null));
  assert(__CPROVER_rw_ok(nondet));
  assert(!__CPROVER_rw_ok(nondet));
  assert(__CPROVER_rw_ok(constant)); 
  assert(__CPROVER_rw_ok(stack, 5));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(invalid));

  free(heap);
  assert(!__CPROVER_is_invalid_pointer(heap));
  assert(__CPROVER_DYNAMIC_OBJECT(heap));
  assert(__CPROVER_r_ok(heap));
  assert(!__CPROVER_r_ok(heap));
  assert(__CPROVER_w_ok(heap));
  assert(!__CPROVER_w_ok(heap));
  assert(__CPROVER_rw_ok(heap));
  assert(!__CPROVER_rw_ok(heap));
}
```

`cbmc predicates.c`

Using `_ok` predicates in assumptions: `cbmc assume_predicates.c`

## Auto-objects

An attempt to give semantics to dereferencing non-deterministic
pointers.

```
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
```

`cbmc auto_objects.c`

## Invalid objects

Yet another attempt to give semantics to dereferencing non-deterministic
pointers.

```
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
```

`cbmc invalid_objects.c`

## Assumptions on non-deterministic pointers

```
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
  assert(42 <= z && z <= 43);
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
```

`cbmc nondet_pointers.c`

https://github.com/diffblue/cbmc/pull/6326/files
