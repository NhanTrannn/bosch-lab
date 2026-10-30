/*
 * Lab: signed/unsigned traps and integer division
 * Topic: C
 * Predict before running: Which branch does line A print? What are the values B, C, D, E, F, G, H?
 * Key idea: When an int meets an unsigned int in a comparison/calculation, the int is converted to unsigned (the usual arithmetic conversions). Integer division truncates toward zero.
 * Try next: Write the loop for (unsigned i = 5; i >= 0; i--) and predict when it stops (remember Ctrl+C).
 * Intentional compiler warning: -Wsign-compare on lines A and C (the trap you are meant to see).
 */
#include <stdio.h>
#include <string.h>

int main(void)
{
    if (-1 < 1U)
        printf("A: -1 < 1U\n");
    else
        printf("A: -1 >= 1U\n");

    unsigned int u = 3;
    printf("B: u - 5 = %u\n", u - 5);

    int i = -1;
    unsigned int k = 1;
    printf("C: i > k ? %d\n", i > k);

    printf("D: 7/2=%d  -7/2=%d  -7%%2=%d  7%%-2=%d  7/2.0=%.1f\n",
           7 / 2, -7 / 2, -7 % 2, 7 % -2, 7 / 2.0);

    char s[] = "abc";
    printf("E: sizeof(\"abc\")=%zu sizeof(s)=%zu strlen(s)=%zu\n",
           sizeof("abc"), sizeof(s), strlen(s));

    unsigned char c = 255;
    c++;
    printf("F: c = %d\n", c);

    short sh = -1;
    printf("G: (unsigned short)sh = %u\n", (unsigned short)sh);

    printf("H: int=%zu long=%zu ptr=%zu char=%zu\n",
           sizeof(int), sizeof(long), sizeof(void *), sizeof(char));
    return 0;
}
