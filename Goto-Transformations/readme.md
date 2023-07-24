# Goto Transformations

## Table of Contents

TODO: Focus on most important stuff (no mmio).
TODO: Add link to implementation.

1. [Introduction](##Introduction)
2. [Symex-Ready Goto Transformations](##"Symex-Ready\ Goto\ Transformations")

## Introduction

This is a live tutorial on transformations applied to the Goto program by
various tools in our suite of tools. We're going to look:

1. Symex-Ready Goto Transformations,
2. (Optional) Transformations applied by `cbmc`, and
3. Transformations applied by `goto-instrument`.

All of the tools that we are demoing today have been built out of `develop`
at the time when `cbmc-5.88.0` was released.

## Symex-Ready Goto Transformations

These are transformations that need to be performed to bring a binary
to a `symex-ready` state - that is, get it into a form that can be
admitted by `symex` without any expected errors (invariants about the
structure of instructions and unlowered expressions as an example).

These transformations have a sequencing dependency, and they are presented
here in the order in which they are performed:

### Remove Assembly

This transformation detects code blocks containing `inline asm` and rewrites
the assembly instructions into `goto-instructions` that have the same semantics.

```c
// original
asm volatile (
  "mov %1, %0; add $1, %0"
  : "=r" (result)
  : "r" (input));

// transformed
ASSIGN main::1::result := main::1::input
ASSIGN main::1::result := main::1::result + 1
```

It's worth mentioning that the tool currently supports doing so for the `x86`
and `Power` architectures.

Unfortunately, I cannot demo this on my local machine, as it's a different
architecture, which means that initialisation for an `aarch64` configuration
means that the system knows that my local assembly flavour would be arm, and
skips the assembly instructions when analysed under this system.

Where you to try this out however, you would want to run something like the
following instruction:

```sh
$ cbmc --show-goto-functions listings/remove_asm.c
```

### Linking to standard libraries

This transformation is looking for function calls inside the goto-program,
where the body of the function is missing and searches the CPROVER library
(under `src/ansi-c/library/`) for a function matching the signature and includes
it in the goto-model.

This usually shows up in the output when the step `Adding CPROVER library` is
done. An actual example would look like this:

```sh
$ cbmc --show-goto-functions listings/linking_cprover_lib.c
[...]
Adding CPROVER library (arm64)
file <builtin-library-fabs> line 2: warning: implicit function declaration "fabs"
old definition in module linking_cprover_lib file listings/linking_cprover_lib.c line 5 function main
signed int (void)
new definition in module <built-in-library> file <builtin-library-fabs> line 2
double (double d)
[...]
```

### Remove Function Pointers

This transformation detects function calls of a dereferenced pointer, and at
that point, it looks up all possible targets of the pointer dereference (it
does that by looking at the function pointer signature, and matching all the
functions with a similar syntax), and substitutes the original expression with
a jump table:

```c
// Original:
int (*fptr_t)(int) fptr;
// ...
assert(fptr(10) == 11);

// Transformed:
if (fptr == &f) f(10)
else if (fptr == &g) g(10)
else if (fptr == &o) o(10)
else assert(false);
```

Let's have a look at a live demo of this: [`remove_function_pointers.c`](listings/remove_function_pointers.c)

Let's examine the `goto-functions` produced by `cbmc`:

```sh
$ just --show remove-function-pointers
[...]
$ just remove-function-pointers
[...]
```

This transformation is implemented in `src/goto-programs/remove_function_pointers.{c++, h}`
and it works in pretty much the expected way:

1. It goes through all functions/instructions of those functions.
2. If it finds a function call with a target being a dereference expression,
   it then dispatches into the handling function.
3. The handling function goes through all the functions in the model, and
   identifies the ones with similar types for the parameters and the return
   type, and then
4. Construct the `if (ptr == &f) goto 1 - 1: f(arg)` expressions and builds a new
   code block that forms a dispatch table.
5. Substitute the original instruction with the codeblock containing the dispatch
   table.

### Instrument preconditions

This transformation is moving function preconditions (which are in the form
of `__CPROVER_precondition`s in the first few instructions of the function body)
before call sites of the functions that are instrumented.

Let's have a look:

```c
#include <assert.h>

int div(int a, int b)
{
    __CPROVER_precondition(b != 0, "Can't divide by zero");
    __CPROVER_precondition(a != 0, "Dividend expected to not be zero");
    return a / b;
}

int main()
{
    int a;
    int b;
    assert(div(a, b) != 0);
}
```

```sh
$ just --show instrument-preconditions
[...]
$ just instrument-preconditons
[...]
```

The way this works is by going through all the function calls in the goto-program,
finding whether the particular function at that call site has any preconditions, and
if it does, it inserts the preconditions before the call-site.

After it has done so for every function, the it iterates through the function map
again, finding any functions containing preconditions, and removing them.

### Remove Returns

This transformation eliminates return statements from the program, and in their
place it substitutes for reads/writes to global variables. The transformation
looks like this:

```c
// original
int foo()
{
   return 1;
}

r = foo();

// transformed (symbolically, the signature of the function isn't really changed)
int foo_return;
void foo()
{
   foo_return = 1;
}

foo();
r = foo_return;
```

To see it in GOTO, let's try the following:

```sh
$ binaries/cbmc --show-goto-functions listings/remove_return.c | tail -33
foo /* foo */
        // 17 file listings/remove_return.c line 3 function foo
        SET RETURN VALUE (foo::b = 2 ? 4 : 8)
        // 18 file listings/remove_return.c line 4 function foo
        END_FUNCTION

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

main /* main */
        DECL main::$tmp::return_value_foo : signedbv[32]
        CALL main::$tmp::return_value_foo := foo(main::1::nondet)
        ASSIGN main::1::res := main::$tmp::return_value_foo
        ASSERT main::1::res ≠ 4 // expected false, path exists for res == 4
        //...
```

This operation also has a dual in the form of `restore_returns()`

### Remove/Lower Vector Typed Expressions

### Remove/Lower Complex Typed Expression

### Rewrite Unions

This transformation rewrites union member reads as `byte_extract` expressions
and union member writes as `byte_update` expressions. So for code like:

```c
typedef union uni {
    int a;
    int b;
} uni;

int main() {
    uni u = {.a = 3, .b = 4 };
    u.a = 5;
    __CPROVER_assert(u.a != 4, "expected failure, a is 5");
}
```

We get:

```sh
$ binaries/cbmc --show-goto-functions listings/rewriting_union.c
[...]
        DECL main::1::u : union tag-uni
        ASSIGN main::1::u := byte_update_little_endian(side_effect statement="nondet" is_nondet_nullable="1", 0, 4, union tag-uni)
        ASSIGN byte_extract_little_endian(main::1::u, 0, signedbv[32]) := 5
        ASSERT byte_extract_little_endian(main::1::u, 0, signedbv[32]) ≠ 4 // expected failure, a is 5
[...]
```

The reason for this transformation is to facilitate presenting a unified interface
to symex (with `byte_update`).

### goto_check_c

This is actually a series of transformations relating to C style language semantics.
A lot of the instrumentations added by a flag are introduced at this point, after
checking if the flag has been set up appropriately (`bounds-check`, `enum-range-check`,
`pointer-checks`, `integer-overflow-checks`, etc).

Let's have a look at an instrumentation added this way:

```sh
$ just --show goto-checkc-div-zero
[...]
$ just goto-checkc-div-zero
[...]
42a43,44
>         ASSERT ¬(*div::b = 0) // division by zero in *a / *b
>         // 17 file listings/div_zero.c line 3 function div
44c46
<         // 17 file listings/div_zero.c line 4 function div
---
>         // 18 file listings/div_zero.c line 4 function div
```

### Adjust Float Expressions

### Goto Functions Update

### Add Failed Symbols

### Remove Skip Instructions

This transformation is removing `SKIP` instructions at the GOTO level. These are
usually the outcome of some other transformations eliminating some instructions.

Symex doesn't handle `SKIP` instructions, aside from treating them as `LOCATION`
instructions, which are instructions semantically equivalent to `SKIP`s, but preserved
after various transformations in the program text.

### Label Properties

This transformation is responsible operates only on `ASSERT` instructions,
and is responsible for setting up the identifiers of the various assertions
in the program.

It does so by fixing a name for the assertion by first prefixing the enclosing
function's name, then the property's class, and then attaching the current value
of a counter for them.

As an example:

```sh
$ binaries/cbmc --bounds-check listings/demo_prop_names.c
CBMC version 5.88.0 (cbmc-5.88.0) 64-bit arm64 macos
Parsing /tmp/weird30.c
[...]

** Results:
/tmp/weird30.c function main
[main.array_bounds.1] line 7 array 'A' lower bound in A[(signed long int)nondet]: SUCCESS
[main.array_bounds.2] line 7 array 'A' upper bound in A[(signed long int)nondet]: SUCCESS
[main.assertion.1] line 8 expected false: FAILURE
[main.assertion.2] line 9 expected false: FAILURE

** 2 of 4 failed (2 iterations)
VERIFICATION FAILED
```
