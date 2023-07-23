# Goto Transformations

## Table of Contents

TODO: Focus on most important stuff (no mmio).


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
