int main() {
  int input = 1;
  int result;

  // Copies `input` to `result` and adds 1 to it.
  asm volatile (
  "mov %1, %0; add $1, %0"
  : "=r" (result)
  : "r" (input));

  __CPROVER_assert(result == 1, "expected result to be 1");
}
