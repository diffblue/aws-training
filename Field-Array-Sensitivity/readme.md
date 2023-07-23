# Field and Array Sensitivity

## Table of Contents

1. [Introduction](##Introduction)

## Introduction

This is a live tutorial on how field and array senstivitiy works in Goto-Symex.
We're going to look at:

1. Motivation for field sensitivity
2. Purpose of field/array cell sensitivity
3. Array cell sensitivity
4. Implementation details

All of the tools that we are demoing today have been built out of `develop`
at the time when `cbmc-5.88.0` was released.

## Motivation for field sensitivity

Running example:

```
struct STRUCT
{
  int a, b, c, d, e, f, g, h;
};  

void main()
{
  struct STRUCT s;
  s.a = 1;
  s.b = s.a;
  s.c = s.b;
  s.d = s.c;
  s.e = s.d;
  s.f = s.e;
  s.g = s.f;
  s.h = s.g;
  assert(s.a != s.h);
}
```

`cbmc field-sensitivity.c`

What would we expect the SSA to look like?

`cbmc field-sensitivity.c --show-vcc`

### Let's emulate non-field sensitivity

`cbmc field-sensitivity.c --show-vcc -DEMULATE_NON_FIELD_SENSITIVE --no-propagation`

## Purpose of field/array cell sensitivity

- Enable constant propagation for struct fields
- Allows us to determine conditions and thus cut branches
- May reduce symex time (considering overhead)
- Reduces SSA size
- Reduces clauses
- May speed up SAT solver

## Array cell sensitivity

Example:
```
void main()
{
  int x[SIZE];
  x[0] = 1;

  for(int i=1; i<SIZE; ++i)
  {
    x[i-1] = x[i];
  }

  assert(x[SIZE-1] != 1);
}
```

`cbmc array-sensitivity.c`

### Without array sensitivity

Build CBMC with `ENABLE_ARRAY_FIELD_SENSITIVITY` disabled:

`./cbmc-5.88.0-no-array-sensitivity array-sensitivity.c --show-vcc`

### With array sensitivity

`cbmc array-sensitivity.c --show-vcc`

Propagation doesn't work? Could be a bug.

## Implementation details

See `goto-symex/field_sensitivity.h`, 
https://diffblue.github.io/cbmc/classfield__sensitivityt.html#details

