/*
 * Lab: The three kinds of const with pointers
 * Topic: C
 * Predict before running: What does the program print? Uncomment the commented-out lines in main one at a time: which compile, which fail, and with what error?
 * Key idea: Read right to left: a const before the * locks the pointed-to data, a const after the * locks the pointer itself.
 * Try next: Try int *q = p1; (dropping const) and see what the compiler warns.
 */
#include <stdio.h>

int main(void)
{
    int a = 1, b = 2;

    const int *p1 = &a;          /* (1) */
    int *const p2 = &a;          /* (2) */
    const int *const p3 = &a;    /* (3) */

    p1 = &b;
    /* *p1 = 10; */

    *p2 = 10;
    /* p2 = &b; */

    /* *p3 = 20; */
    /* p3 = &b; */

    printf("a = %d, b = %d, *p1 = %d, *p2 = %d, *p3 = %d\n",
           a, b, *p1, *p2, *p3);
    return 0;
}
