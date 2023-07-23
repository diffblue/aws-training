#include <assert.h>

int div(int a, int b)
{
    __CPROVER_precondition(b != 0, "Can't divide by zero");
    __CPROVER_precondition(a != 0, "Dividend expected to not be zero");
    return a / b;
}

int main()
{
    int a;
    int b;
    assert(div(a, b) != 0);
}
