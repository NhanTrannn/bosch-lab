/*
 * Lab: Pointer arithmetic, arrays and decay
 * Topic: C
 * Predict before running: How do sizeof(arr) in main and sizeof(a) in f differ? What does *p++ print? How many bytes apart are pb + 1 and pb?
 * Key idea: An array passed to a function "decays" to a pointer. int *pa[3] is an array of 3 pointers; int (*pb)[3] is a pointer to an array of 3 ints.
 * Try next: Change pb to int (*pb)[2][3] = &m; and print (char *)(pb + 1) - (char *)pb.
 * Intentional compiler warning: -Wsizeof-array-argument in function f (exactly what this lab wants to show).
 */
#include <stdio.h>

void f(int a[10])
{
    printf("in f   : sizeof(a)   = %zu\n", sizeof(a));
}

int main(void)
{
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;

    printf("in main: sizeof(arr) = %zu\n", sizeof(arr));
    f(arr);

    printf("*(p + 2) = %d\n", *(p + 2));
    printf("p[3] = %d, 3[arr] = %d\n", p[3], 3[arr]);
    printf("*p++ = %d, ", *p++);
    printf("then *p = %d\n", *p);
    printf("&arr[4] - &arr[0] = %td\n", &arr[4] - &arr[0]);

    int m[2][3] = {{1, 2, 3}, {4, 5, 6}};
    int x = 7, y = 8, z = 9;
    int *pa[3] = {&x, &y, &z};
    int (*pb)[3] = m;

    printf("sizeof(pa) = %zu, sizeof(pb) = %zu\n", sizeof(pa), sizeof(pb));
    printf("*pa[1] = %d, pb[1][2] = %d\n", *pa[1], pb[1][2]);
    printf("(char *)(pb + 1) - (char *)pb = %td\n", (char *)(pb + 1) - (char *)pb);
    return 0;
}
