#include <assert.h>

void main(){
  __CPROVER_field_decl_local("shadow", (_Bool)0);
  __CPROVER_field_decl_global("shadow", (_Bool)0);

  char x = 0;

  __CPROVER_set_field(&x, "shadow", 1);

  _Bool shadow_value = __CPROVER_get_field(&x, "shadow");
  assert(shadow_value == 0);
}
