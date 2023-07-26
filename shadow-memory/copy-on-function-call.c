void foo(int arg) {
   assert(__CPROVER_get_field(&arg, "taint") == 1); // Passes as the value of arg is copied from x
}

int main() {
   __CPROVER_field_decl_local("taint", (_Bool)0);
   __CPROVER_field_decl_global("taint", (_Bool)0);
   int x;
   x = 42;

   __CPROVER_set_field(&x, "taint", 1);

   foo(x); // SUCCESS
   return 0;
}
