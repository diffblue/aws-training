#include <assert.h>

struct S {
  unsigned long f1;
  char f2;
};

void main(){

  __CPROVER_field_decl_local("shadow", (_Bool)0);
  __CPROVER_field_decl_global("shadow", (_Bool)0);

  struct S s;

  __CPROVER_set_field(&s.f1, "shadow", 1);

  assert(__CPROVER_get_field(&s, "shadow") == 0);
}
