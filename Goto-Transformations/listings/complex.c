#include <assert.h>

int main()
{
  // C99 Complex number, https://en.cppreference.com/w/c/numeric/complex#:~:text=The%20C%20programming%20language%2C%20as,float%20complex%2C%20long%20double%20complex.
  _Complex c; // this is usually '_Complex double'
  c=1.0i+2;

  assert(__real__ c == 2);
  assert(__imag__ c == 1);
}
