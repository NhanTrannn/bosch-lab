/*
 * Lab: Variable scope, shadowing and static inside a loop
 * Topic: C
 * Predict before running: Which number does each x= line print? What do the three calls to f() print? How do s and t print in the last for loop?
 * Key idea: A variable in an inner block shadows an outer variable with the same name. A static local is initialized ONCE and lives for the whole program; an automatic variable is re-initialized every time the block is entered. Uninitialized static/global variables are 0 (they live in .bss).
 * Try next: Compile with -Wshadow to see the warnings. Remove the "= 0" from int m → garbage value (UB); try it under valgrind.
 */
#include <stdio.h>

int x = 1;

void f(void)
{
    static int n;   /* = 0 */
    int m = 0;
    n++;
    m++;
    printf("f: n = %d, m = %d\n", n, m);
}

int main(void)
{
    printf("x = %d\n", x);
    int x = 2;
    {
        int x = 3;
        printf("x = %d (inner block)\n", x);
        {
            extern int x; /* back to the global variable */
            printf("x = %d (extern)\n", x);
        }
    }
    printf("x = %d\n", x);

    for (int i = 0; i < 3; i++)
        f();

    for (int i = 0; i < 3; i++) {
        static int s = 10;
        int t = 10;
        s++;
        t++;
        printf("s = %d, t = %d\n", s, t);
    }
    return 0;
}
