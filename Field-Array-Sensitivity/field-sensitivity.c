struct STRUCT
{
  int a, b, c, d, e, f, g, h;
};  

extern int nondet();

void main()
{
  struct STRUCT x;
  x.a = 1;

  #ifdef EMULATE_NON_FIELD_SENSITIVE
  x = x;
  #endif

  x.b = x.a;

  #ifdef EMULATE_NON_FIELD_SENSITIVE
  x = x;
  #endif

  x.c = x.b;
 
  #ifdef EMULATE_NON_FIELD_SENSITIVE
  x = x;
  #endif

  x.d = x.c;

  #ifdef EMULATE_NON_FIELD_SENSITIVE
  x = x;
  #endif

  x.e = x.d;

  #ifdef EMULATE_NON_FIELD_SENSITIVE
  x = x;
  #endif

  x.f = x.e;

  #ifdef EMULATE_NON_FIELD_SENSITIVE
  x = x;
  #endif

  x.g = x.f;

  #ifdef EMULATE_NON_FIELD_SENSITIVE
  x = x;
  #endif

  x.h = x.g;

  #ifdef EMULATE_NON_FIELD_SENSITIVE
  x = x;
  #endif

  assert(x.h != 1);
}
