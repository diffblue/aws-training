// #include <math.h> // NOTE: uncomment to see more CPROVER_library functions included

int main() {
    float original = -0.329f;
    float absolute = fabs((double)original);
    float inverted = -original;
    __CPROVER_assert(absolute != inverted, "expected absolute value of original to be inverted");
}
