int main()
{
    int nondet;
    __CPROVER_assume(nondet >= 0 && nondet < 3);
    unsigned char A[4];
    unsigned char b;
    A[nondet] = b; 
    __CPROVER_assert(A[nondet] != 'z', "expected false");
    __CPROVER_assert(A[nondet] == 'o', "expected false");
}
