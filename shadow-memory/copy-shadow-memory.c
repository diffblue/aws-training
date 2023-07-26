#include <assert.h>

void main(){
  __CPROVER_field_decl_local("shadow", (_Bool)0);
  __CPROVER_field_decl_global("shadow", (_Bool)0);

  int x = 42;
  int y = 0;

  __CPROVER_set_field(&x, "shadow", 1);
  __CPROVER_set_field(&y, "shadow", 0);

  y = x;

  assert(x == y);
  assert(x == 42);
  _Bool x_shadow_value = __CPROVER_get_field(&x, "shadow");
  assert(x_shadow_value == 1);
  _Bool y_shadow_value = __CPROVER_get_field(&y, "shadow");
  assert(y_shadow_value == 0);
}
