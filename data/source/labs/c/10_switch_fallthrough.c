/*
 * Lab: switch without break
 * Topic: C
 * Predict before running: For x from 0 to 4, what does each line print?
 * Key idea: Without a break, execution continues into the cases below (fall-through). default does not have to be last.
 * Try next: Add a break after "three " and predict the x = 3 line again.
 * Intentional compiler warning: -Wimplicit-fallthrough (deliberate, so you see where the compiler warns).
 */
#include <stdio.h>

void test(int x)
{
    printf("x = %d: ", x);
    switch (x) {
    case 1:
        printf("one ");
    case 2:
        printf("two ");
        break;
    case 3:
        printf("three ");
    default:
        printf("default ");
    case 4:
        printf("four ");
    }
    printf("\n");
}

int main(void)
{
    for (int i = 0; i <= 4; i++)
        test(i);
    return 0;
}
