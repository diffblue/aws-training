int forbidden(int x);

int main() {
    int x = 4;
    int y = forbidden(y);

    __CPROVER_assert(y != 5, "should never be 5");
}
