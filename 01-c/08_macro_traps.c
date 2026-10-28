/*
 * Lab: Macro traps
 * Topic: C
 * Predict before running: What is the value on each line? After SQ_OK(i++) and MAX(j++, 3), what are i and j?
 * Key idea: Macros are pure text substitution. Always parenthesize each parameter and the whole expression; an argument with side effects may be evaluated more than once.
 * Try next: Rewrite SQ and MAX as static inline functions, run again and compare i and j.
 * Intentional compiler warning: -Wsequence-point on the SQ_OK(i++) line (deliberate undefined behavior).
 */
#include <stdio.h>

#define SQ(x)      x * x
#define SQ_OK(x)   ((x) * (x))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))
#define DOUBLE(x)  x + x

int main(void)
{
    int a = 3;
    printf("SQ(a + 1)     = %d\n", SQ(a + 1));
    printf("SQ_OK(a + 1)  = %d\n", SQ_OK(a + 1));
    printf("10 / SQ(2)    = %d\n", 10 / SQ(2));
    printf("10 / SQ_OK(2) = %d\n", 10 / SQ_OK(2));
    printf("DOUBLE(5) * 2 = %d\n", DOUBLE(5) * 2);

    int i = 5;
    int r = SQ_OK(i++);
    printf("SQ_OK(i++) = %d, i = %d\n", r, i);

    int j = 5;
    int m = MAX(j++, 3);
    printf("MAX(j++, 3) = %d, j = %d\n", m, j);
    return 0;
}
