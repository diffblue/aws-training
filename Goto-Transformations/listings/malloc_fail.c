#include <stdlib.h>

int main()
{
    int *a = malloc(sizeof(int));
    __CPROVER_assert(a != NULL, "can fail");
    *a = 5;
    __CPROVER_assert(*a == 5, "expected a to be 5");
}
