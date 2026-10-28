/*
 * Lab: Function pointers, arrays of function pointers and callbacks (qsort)
 * Topic: C
 * Predict before running: What do the first three lines print? Is there any difference between fp(1, 2) and (*fp)(1, 2)? What order is the array in after qsort?
 * Key idea: A function name decays to a function pointer (like an array). Calling fp(x) or (*fp)(x) is the same. A table of function pointers replaces a long switch (state machines, command tables) — very common in embedded code.
 * Try next: Write a comparator that sorts ascending. Read the declarations: what are int *(*g)(int) and int (*h[3])(void)?
 */
#include <stdio.h>
#include <stdlib.h>

static int add(int a, int b) { return a + b; }
static int sub(int a, int b) { return a - b; }
static int mul(int a, int b) { return a * b; }

typedef int (*op_t)(int, int);

static int desc(const void *x, const void *y)
{
    int a = *(const int *)x, b = *(const int *)y;
    return (a < b) - (a > b); /* avoid b - a, which can overflow */
}

static void apply(int *v, int n, int (*fn)(int))
{
    for (int i = 0; i < n; i++)
        v[i] = fn(v[i]);
}

static int square(int x) { return x * x; }

int main(void)
{
    op_t ops[] = {add, sub, mul};
    const char *sym = "+-*";
    for (int i = 0; i < 3; i++)
        printf("7 %c 3 = %d\n", sym[i], ops[i](7, 3));

    int (*fp)(int, int) = &add;
    printf("fp(1, 2) = %d, (*fp)(1, 2) = %d, fp == add ? %d\n", fp(1, 2), (*fp)(1, 2), fp == add);
    printf("sizeof(fp) = %zu\n", sizeof(fp));

    int v[] = {5, 1, 4, 2, 3};
    int n = sizeof(v) / sizeof(v[0]);
    qsort(v, n, sizeof(v[0]), desc);
    for (int i = 0; i < n; i++)
        printf("%d ", v[i]);
    printf("\n");

    apply(v, n, square);
    for (int i = 0; i < n; i++)
        printf("%d ", v[i]);
    printf("\n");
    return 0;
}
