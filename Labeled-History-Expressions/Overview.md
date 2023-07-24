# Labeled History Expressions

### Goal

Have the capability to determine the value of a given expression at a given label.

## Proposed syntax:
```
__CPROVER_at(expr, label)
```
where `expr` is the expression and `label` is the label at which the value of `expr` should be determined.

A trivial example:
```
int x = 1;
int y = 4;
label_1: y++;
x = y * 2;
assert(__CPROVER_at(x < 2, label_1));
```
In this example the expression `x < 2` should hold since at `label_1` the value of `x` was `1`.


### Aside: This is conceptually similar equivalent to inserting a new variable:
```
int x = 1;
int y = 4;
label_1: {int cprover_at_1 = x < 2; y++;};
x = y * 2;
assert(cprover_at_1);
```
which also clarifies/simplifies some of the concepts/limitations below.


## Limitations

Below are some limitations on how `__CPROVER_AT(...)` can be used:

1. The label and the __CPROVER_at expression must occur in the same function. (Is this strictly necessary?)
2. The scope in which the label appears must include the scope of the referencing expression.
3. Any variables mentioned in the expression must be in scope at both the location of the expression and the labeled location. Note that this must be the same variables!
4. The labeled program location must be a graph dominator of the location of the expression. That is, there is no way to reach the expression without passing through the labeled location.

### Bad Examples

Incorrect scope
```
int x = 1;

void func(int y) {
  start_func: y = x << 2;
  int x = 0;
  // ...
  assert(__CPROVER_AT(x > 0, start_func));
}
```
since the scope of `x` is different at the label and the use of `__CPROVER_AT(...)`.

Control flow problem
```
void func(int y) {
  if (y % 2 == 0) {
    true_branch: y = y / 2;
  } else {
    y++;
  }
  assert(__CPROVER_AT(y != 3, true_branch))
}
```
since `true_branch` may not have been visited (is not graph dominator of the `__CPROVER_AT(...)`).


## Notes

Validity assertions and checks need to be implemented for the variables current at the label. Consider
```
__CPROVER_at(array[index], label)
```
the validity/bounds check of `index` for `array` need to be inserted/checked at the last execution of `label`, not at the current time. For an example of this, consider:
```
  buffer = malloc(1024);
  memcpy(buffer, input, 1024);
  int index = 0;
  start_processing: if (buffer[index] > 0) {
  ...
  free(buffer);
  assert(__CPROVER_AT(buffer[index] == 0, start_processing));
```
while `buffer` has been freed at the assertion, the memory was valid and (ignoring the `...`) the value of `index` was within bounds at the time we executed `start_processing`.

## Acceptable Limitations

It is sufficient if the labels/expressions only occur outside of loops (broadly defined, includes recursive functions, etc.).

