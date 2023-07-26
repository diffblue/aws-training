# Symbolic Shadow Memory

## Background material

- The training materials (including this slide).
   https://github.com/diffblue/aws-training
- Shadow Memory CBMC branch (Experimental):
   `shadow-memory-dup`
   https://github.com/peterschrammel/cbmc/tree/shadow-memory-dup
- Peter's Presentation:
   https://www.youtube.com/watch?v=jTEcM9WCtJo
- Shadow Memory paper:
   https://dl.acm.org/doi/abs/10.1145/3551349.3559523

## Symbolic Shadow Memory

CBMC will come with symbolic shadow memory capabilities.

Shadow memory is a structure that "shadows" the memory of the program.
Used to store information on memory locations in the program to track and express
properties other than the one depending on value stored in the memory.

Simplifies instrumentation for the verification of many properties including:

- Taint analysis,
- Data race conditions (LazyCSeq),
- Stacked borrows for Rust verification (Remi),
- Tracking meta data about registers and stack locations for virtual machines/interpreters (Remi),
- etc...

### How Shadow Memory works

It replicates the memory objects into shadow memory locations where each byte of the 
original object carries the shadow-value in it.

```
variable            memory             shadow memory
char v         -->  ['v']          --> [0x1]
int16 x        -->  [0xA,0xB]      --> [0x1,0x1]
char v[3]      -->  ['h','i','\0'] --> [0x1,0x2,0x2]
struct S {
   char f1;
   int16 fw;
} s            -->  ['a',0xA,0xB]  --> [0x1,0x2,0x2]
```

```
variable        |   memory           |  shadow memory 1  |   shadow memory 2
char v         -->  ['v']          ---> [0x1]
                                     \---------------------> [0x2]
int16 x        -->  [0xA,0xB]      ---> [0x1,0x1]
                                     \---------------------> [0x0,0x0]
char v[3]      -->  ['h','i','\0'] ---> [0x1,0x2,0x2]
                                     \---------------------> [0x3,0x1,0x1]
```

### How to run:

Simply run `cbmc` on a program using shadow memory.

### Shadow Memory API

Shadow memory can be accessed using the following primitives:

- `__CPROVER_field_decl_global(name, (type)value)`,
  `__CPROVER_field_decl_local(name, (type)value)`:
   Declare a global/local shadow memory field with `name`, `type` and initial `value`.
- `__CPROVER_get_field(address, name)`:
   Get value of field with `name` for memory at `address`.
- `__CPROVER_set_field(address, name, value)`
   Set field with `name` to `value` for memory at `address`.

### Usage (somewhat taint analysis example)

Declare a local and global shadow memory named "taint" of type `_Bool` and initial value `0`.
*Note* here the shadow memory "taint" type is inferred `_Bool`:

```C
__CPROVER_field_decl_local("taint", (_Bool)0);
__CPROVER_field_decl_global("taint", (_Bool)0);
```

Get the value stored in the shadow memory "taint" at pointer `x`:

```C
int x;

__CPROVER_get_field(&x, "taint"); // 0
```

Set the value stored in the shadow memory "taint" at pointer `x` to the value `1`:

```C
__CPROVER_set_field(&x, "taint", 1);
```

Get the value stored in the shadow memory "taint" at pointer `x`:

```C
__CPROVER_get_field(&x, "taint"); // 1
```

Shadow memory is associated to the pointer, not the value, so it is not propagated on copy:

```C
int *x, *y; // assume x and y are valid;
*x = 42;
*y = 0;

__CPROVER_set_field(x, "taint", 0);
__CPROVER_set_field(y, "taint", 1);

*y = *x; // *x == *y == 42

__CPROVER_get_field(x, "taint"); // 0
__CPROVER_get_field(y, "taint"); // 1

__CPROVER_set_field(y, "taint", 
                    __CPROVER_get_field(x, "taint")); // y is taint if x is

__CPROVER_get_field(x, "taint"); // 0
__CPROVER_get_field(y, "taint"); // 0
```

Shadow memory values can be expressions (provided they are byte-sized):

```C
__CPROVER_field_decl_local("char-shadow", (char)0);
char c;

c = 'a';

__CPROVER_set_field(&c, "char-shadow", c + 1); // sets c's shadow value to 'b'

c = 'c';

__CPROVER_get_field(&c, "char-shadow"); // 'b'
```

Shadow memory values can also be nondeterministic:

```C
_Bool is_tainted;
char c;

__CPROVER_set_field(&c, "taint", is_tainted);

assert(__CPROVER_get_field(&c, "taint"));  // FAIL
assert(!__CPROVER_get_field(&c, "taint")); // FAIL
```

Shadow memory values are replicated to be able to support byte-extraction:

```C
int16 x;

__CPROVER_set_field(&x, "taint", 0x1); // this is replicated  [0x1,0x1]

__CPROVER_get_field(((char) &x) + 1, "taint"); // 0x1 as all x is tainted
```

Shadow memory field are aggregated on get operations:

```C
int16 x;

__CPROVER_set_field(&x, "taint", 0x0);              // [0x0,0x0]

__CPROVER_set_field(((char) &x) + 1, "taint", 0x1); // [0x0,0x1]

__CPROVER_get_field(&x, "taint"); // 0x1 as part of it is tainted
```
In this case we can see that the various bytes of the shadow memory are 
aggregated by using the `max` operation.

Shadow memory can be efficiently set by using replication:

```C
struct S {
   int id;
   int passwd;
} login_values;

__CPROVER_set_field(&login_values, "taint", 0x1);

__CPROVER_get_field(&login_values, "taint");          // 0x1
__CPROVER_get_field(&login_values.id, "taint");       // 0x1
__CPROVER_get_field(&login_values.passwd, "taint");   // 0x1

__CPROVER_set_field(&login_values.id, "taint", 0x0);  // id is not tainted

__CPROVER_get_field(&login_values.id, "taint");       // 0x0
__CPROVER_get_field(&login_values.passwd, "taint");   // 0x1
__CPROVER_get_field(&login_values, "taint"); // 0x1 as the struct contains tainted values
```

Multiple shadow memory can be used in the same program:

```C
__CPROVER_field_decl_local("taint_for_user", (_Bool)0);
__CPROVER_field_decl_local("taint_for_attacker", (_Bool)0);

int passwd;

__CPROVER_set_field(&passwd, "taint_for_user", 0x0);     // user is allowed access passwd
__CPROVER_set_field(&passwd, "taint_for_attacker", 0x1); // attacker is not allowed to access passwd
```

Full taint analysis example

### Options

`--copy-shadow-memory`: automatically propagate the shadow memory on value-copy.

*NOTE* that using this means the shadow memory is ALWAYS copied on value copy.

`cbmc --copy-shadow-memory <input program>` behaves as follows:

```C
int *x, *y; // assume x and y are valid;
*x = 42;
*y = 0;

__CPROVER_set_field(x, "taint", 0);
__CPROVER_set_field(y, "taint", 1);

*y = *x; // *x == *y == 42

__CPROVER_get_field(x, "taint"); // 0
__CPROVER_get_field(y, "taint"); // 0
```

Similarly the behavior happens on function call parameters:
```C
void foo(int arg) {
   assert(__CPROVER_get_field(&arg, "taint") == 1); // Passes as the value of arg is copied from x
}

int main() {
   __CPROVER_field_decl_local("taint", (_Bool)0);
   int x;
   x = 42;

   __CPROVER_set_field(&x, "taint", 1);

   foo(x); // SUCCESS
   return 0;
}
```

## Documentation and examples

- Example files in:
   - `regression/cbmc-shadow-memory`.

## Ongoing work:
- Add the prototype implementation to CBMC.

## Future works:
- Let's write this together!
