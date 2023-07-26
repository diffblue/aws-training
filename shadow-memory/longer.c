#include <assert.h>

void main(){

  __CPROVER_field_decl_local("shadow", (_Bool)1);
  __CPROVER_field_decl_global("shadow", (_Bool)1);

  int *x;

  __CPROVER_set_field(&x, "shadow", 1);

  assert(__CPROVER_get_field(&x, "shadow") == 0);
}
