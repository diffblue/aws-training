#include <assert.h>

int main(void) {
  int a[2000];
  
  unsigned int i;

  __CPROVER_assume(i < 2000);
  a[i] = 1;
  assert(a[i] == 0); 
}
