static int counter;

int main()
{
    __CPROVER_assert(counter == 0, "expected false, static init == 0");
}
