typedef union uni {
    int a;
    int b;
} uni;

int main() {
    uni u = {.a = 3, .b = 4 };
    u.a = 5;
    __CPROVER_assert(u.a != 4, "expected failure, a is 5");
}
