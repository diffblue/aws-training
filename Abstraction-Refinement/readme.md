# Abstraction Refinement Loop

## Introduction

This is a live tutorial on what CBMC's `--refine` options do.
We're going to look at:

1. Overview and purpose of refinement options
2. Incremental SAT solving
3. Abstraction refinement loop
4. `--incremental-loop`

All of the tools that we are demoing today have been built out of `develop`
at the time when `cbmc-5.88.0` was released.

## Overview and purpose of refinement options

### `--refine-arrays`

Goal: Speed up "postprocessing" stage.

Add array constraints lazily, i.e. refinement of an over-approximation.

Example:
```
#define SIZE 1000

void main()
{
  int x[SIZE];
  int i;
  for (int i=0; i<SIZE; ++i)
    x[i] = 1;

  int j;
  __CPROVER_assume(0<=j && j<SIZE);
  assert(x[j] != 1);
}
```

Bit-blast arrays: `time cbmc refine-arrays.c --arrays-uf-never`

Use eager array theory: `time cbmc refine-arrays.c --arrays-uf-always`

Use lazy array theory: `time cbmc refine-arrays.c --arrays-uf-always --refine-arrays`

Bug to fix: Refine arrays can actually use simplifier.

### `--refine-arithmetic`

Goal: Speed up expensive arithmetic

Double refinement loop, i.e. over- and under-approximations.

See details in 
http://www.kroening.com/papers/fmcad2009.pdf

Example:
```
#define NUMBER_OF_MULT 14

void main()
{
  long long x;
  __CPROVER_assume(1ll <= x && x < 16ll);

  long long z = 1;
  for (int i=0; i<NUMBER_OF_MULT; ++i)
    z *= x;

  assert(1ll <= z && z < 16ll << (NUMBER_OF_MULT * 4));
}
```

Without abstraction: `time cbmc refine-arithmetic.c`

With abstraction: `time cbmc refine-arithmetic.c --refine-arithmetic`

Similar ideas in http://www.kroening.com/papers/sttt-bv-2008.pdf.

Experimental resuls also in
http://www.kroening.com/papers/facj2017.pdf (Fig. 6 and 7)
Not really advantageous overall - but may depend on benchmark set.

Option `--max-node-refinement` limits the number of refinements.

### `--refine`

Enables the two previous options.

### `--refine-strings`

To enable the String theory solver, which also adds constraints lazily.
For JBMC.
Not discussed here.
See: https://arxiv.org/abs/2302.02381

## Incremental SAT solving

Monotonic formula construction by adding clauses.

Non-monotonic formula construction using assumptions (SAT solver) or push/pop (SMT solver).

See http://www.kroening.com/papers/facj2017.pdf

## Abstraction refinement loop

Implemented in `bv_refinementt`.

## `--incremental-loop`

Incrementally adds unwindings to a specified loop. Useful for reactive loops.

Goal: Don't waste time by trying different `--unwind` values.

See http://www.kroening.com/papers/facj2017.pdf

Implemented in `single_loop_incremental_symex_checkert`.

Example:
```
void main()
{
  int x = nondet();
  __CPROVER_assume(0 <= x && x <= 1);
  while(1)
  {
    x = x + 1;
    assert(x < BOUND);
  }
}
```

Without incremental unwinding: `cbmc incremental-loop.c --unwinding-assertions --unwind 10`

Figuring out the loop id: `cbmc incremental-loop.c --show-loops`

With incremental unwinding: `cbmc incremental-loop.c --unwinding-assertions --incremental-loop main.0`
