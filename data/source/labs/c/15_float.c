/*
 * Lab: Floating point: comparison, rounding, integer division
 * Topic: C
 * Predict before running: Is 0.1 + 0.2 == 0.3? Is f == 0.1? What are x, y and z? (int)-3.99? What does 16777217.0f print?
 * Key idea: double/float are binary, so 0.1 is not exact → compare with an epsilon. float has only ~7 significant digits (24-bit mantissa). Casting to int truncates toward zero.
 * Try next: Print 0.1f with %.20f. Try 1.0 / 0 and 0.0 / 0, then compare nan == nan.
 */
#include <stdio.h>

int main(void)
{
    double d = 0.1 + 0.2;
    printf("0.1 + 0.2 == 0.3 ? %d\n", d == 0.3);
    printf("0.1 + 0.2 = %.17f\n", d);
    double diff = d - 0.3;
    printf("|d - 0.3| < 1e-9 ? %d\n", diff < 1e-9 && diff > -1e-9);

    float f = 0.1f;
    printf("f == 0.1 ? %d, f == 0.1f ? %d\n", f == 0.1, f == 0.1f);

    int x = 7 / 2;
    double y = 7 / 2;
    double z = 7 / 2.0;
    printf("x = %d, y = %.1f, z = %.1f\n", x, y, z);
    printf("(int)3.99 = %d, (int)-3.99 = %d\n", (int)3.99, (int)-3.99);

    float big = 16777217.0f;
    printf("16777217.0f = %.1f\n", big);

    double zero = 0.0;
    double inf = 1.0 / zero, nan = zero / zero;
    printf("inf = %f, nan == nan ? %d, inf > 1e308 ? %d\n", inf, nan == nan, inf > 1e308);
    printf("sizeof: float = %zu, double = %zu, long double = %zu\n", sizeof(float), sizeof(double), sizeof(long double));
    return 0;
}
