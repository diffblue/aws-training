int div(int *a, int *b)
{
    return *a / *b;
}

int main()
{
    int a;
    int b;
    __CPROVER_assert(div(&a, &b) != 10, "expected false");
}
