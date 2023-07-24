# Goto-Symex Engine

## Introduction

This is a live tutorial on how Goto-symex processes a Goto program.
We're going to look at:

1. Symex target equation (SSA)
2. Variable generations
3. CFG exploration
4. Constant propagation
5. Symbolic dereferencing
6. Function pointers
7. Slicing
8. Goto verifiers and incremental goto checkers

All of the tools that we are demoing today have been built out of `develop`
at the time when `cbmc-5.88.0` was released.

## Symex target equation (SSA)

Goto-symex transforms a Goto program into SSA.
This transformation performs
- removal of side effects
- control flow encoding into guards
- bounded unwinding of loops and recursions
- concurrency encoding for threads
I.e. bounded abstract interpretation of the Goto program (CFG) with
product domain of 
- collecting SSA equations
- constant propagation 
- may-points-to.

SSA is then translated to a first-order logic formula.

Example:
```
void main()
{
  int x;
  __CPROVER_assume(-1000 <= x && x <= 1000);
  
  int c;
  if (c)
    x = x < 0 ? -x : x;
  else
    x *= x;

  assert(x >= 0);
}
```

Goto program representation: `cbmc branching.c --show-goto-functions`
SSA representation: `cbmc branching.c --program-only`
Per-assertion SSA representation: `cbmc branching.c --show-vcc`

## Variable generations

Example:
```
#include <pthread.h>

int power_iterative(int base, int exponent)
{
  int current = 1;
  for(int i=0; i<exponent; ++i)
    current *= base;
  return current;
}

int power_recursive(int base, int exponent, int current)
{
  if (exponent == 0)
    return current;
  return power_recursive(base, exponent - 1, current) * base;
}

int current = 1;

void worker(void *arg)
{
  int base = *(int *)arg;
  __CPROVER_atomic_begin();
  current *= base;
  __CPROVER_atomic_end();
}

int power_threaded(int base, int exponent)
{
  pthread_t thread[exponent];
  for(int i=0; i<exponent; ++i)
    pthread_create(&thread[i], NULL, worker, &base);
  for(int i=0; i<exponent; ++i)
    pthread_join(thread[i], NULL);
  return current;
}

void main()
{
  int base;
  __CPROVER_assume(0 <= base && base < 4);
  int exponent;
  __CPROVER_assume(0 <= exponent && exponent < 4);

  int iterative = power_iterative(base, exponent);
  int recursive = power_recursive(base, exponent, 1);
  int threaded = power_threaded(base, exponent);
  assert(iterative == recursive);
  assert(iterative == threaded);
}
```

`cbmc unwinding.c --unwinding-assertions --unwind 4`

`cbmc unwinding.c --unwinding-assertions --unwind 4 --show-vcc | grep current | less`

`cbmc unwinding.c --unwinding-assertions --unwind 4 --show-vcc | less`

## CFG exploration

Worklist algorithm: 
0. put first instruction into worklist
1. take element from worklist
2. process element
3. put successors into worklist
4. Repeat from 1

### Multi-path

Explores if before else and loop iterations before loop exit.

`cbmc branching.c --show-goto-functions`
`cbmc branching.c --show-vcc`

### Single-path

Two strategies
- `cbmc branching.c --show-vcc --paths lifo`
- `cbmc branching.c --show-vcc --paths fifo`

## Constant propagation

Example:
```
void main()
{
  int bound;
  __CPROVER_assume(3 <= bound && bound <= 3);
  int value;
  __CPROVER_assume(42 <= value && value <= 42);
  
  int x = 0;
  int y;
  while (x < bound)
  {
    ++x;
    y = value;
  }
  assert(y != value);
}
```

`cbmc loop.c`

With constant propagation: `cbmc loop.c --program-only`

Without constant propagation: 
`cbmc loop.c --program-only --no-propagation`
`cbmc loop.c --program-only --no-propagation --unwind 5`

Inability to propagate: change upper bound of `value` to `43`:
`cbmc loop.c --program-only`

Inability to propagate and determine unwinding bound: change upper bound of `bound` to `4`:
`cbmc loop.c --program-only`
`cbmc loop.c --program-only --unwind 5`

## Symbolic dereferencing

Uses may-points-to analysis information ("value sets").

Example:
```
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
```

`cbmc dereference.c`
`cbmc dereference.c --show-vcc --no-propagation`

## Function pointers

Handled by goto transformation.

Example:
```
int global;

void f(int farg)
{
  global = 1;
}

void g(int garg)
{
  global = 0;
}

void main()
{
  void (*p)(int);
  __CPROVER_bool c;

  p = c ? f : g;

  p(1);

  assert(global == c);
}
```

`cbmc function-pointers.c --show-goto-functions`

## Slicing

Example:
```
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
```

Simple slice (default): `cbmc slice.c --show-vcc`
Formula slice: `cbmc slice.c --show-vcc --slice-formula`

Other slicers (not covered here):
- reachability slice 
- full slice
- aggressve slice

## Goto verifiers and incremental goto checkers

### Goto verifier

- verification of a goto program
- defines behaviour on finding a property violation
-- property status reporting
-- trace reporting
- uses an incremental goto checker (typically inside a loop)

Implementations:
- Default in CBMC: `all_properties_verifier_with_trace_storaget`  (reports all traces at the end)
- `all_properties_verifiert` (reports traces as violations are found)
- With `--stop-on-fail`: `stop_on_fail_verifiert`
- ...

### Incremental goto checker

- incremental verification until the next property violation
- defines how this verification is done

Implementations:
- Default in CBMC: `multi_path_symex_checkert`
- With `--paths`: `single_path_symex_checkert`
- With `--incremental-loop`: `single_loop_incremental_symex_checkert`
- With `--show-vcc`, etc: `...symex_only_checkert`
- ...
