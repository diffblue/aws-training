#include <assert.h>

typedef int v4si __attribute__((vector_size(16)));

int main()
{
  v4si a = {1, 2, 3, 4};
  v4si b = {5, 6, 7, 8};

  v4si c = a + b;
}
