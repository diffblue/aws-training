int foo(int b)
{
    return b == 2 ? 4 : 8;
}

int main()
{
    int nondet;
    int res = foo(nondet);
    __CPROVER_assert(res != 4, "expected false, path exists for res == 4");
}
