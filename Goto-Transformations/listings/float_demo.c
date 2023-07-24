int main()
{
    float a = 3.10f;
    float b = 2.09f;
    float c = a + b;
    __CPROVER_assert(c != 3.19f, "may fail");
}
