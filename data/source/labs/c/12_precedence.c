/*
 * Lab: Operator precedence and short-circuit evaluation
 * Topic: C
 * Predict before running: What is the value on each line? After the || and && lines, what are i and k? What is arr[1] at the end?
 * Key idea: == and != bind TIGHTER than & | ^; + - bind tighter than << >>; ?: associates right to left. && and || stop early (short-circuit) and contain a sequence point.
 * Intentional compiler warning: -Wparentheses (a & 4 == 4, 1 << 2 + 1) and -Wunused-value (the comma operator).
 * Try next: Rewrite every line that warns with parentheses that express the intent. Try printing i++ + ++i (UB — never write that in real code).
 */
#include <stdio.h>

int main(void)
{
    int a = 6; /* 110b: bit 2 is set */
    printf("a & 4 == 4   -> %d\n", a & 4 == 4);
    printf("(a & 4) == 4 -> %d\n", (a & 4) == 4);
    printf("1 << 2 + 1   -> %d\n", 1 << 2 + 1);
    printf("10 - 2 - 3   -> %d\n", 10 - 2 - 3);
    printf("2 * 3 %% 4    -> %d\n", 2 * 3 % 4);

    int arr[] = {10, 20, 30};
    int *p = arr;
    int v = *p++;  /* *(p++) */
    int w = ++*p;  /* ++(*p): increments arr[1] */
    int y = (*p)++;
    printf("v = %d, w = %d, y = %d, arr[1] = %d\n", v, w, y, arr[1]);

    int q = 5;
    int r = q > 3 ? q < 10 ? 1 : 2 : 3;
    printf("nested ?:   -> %d\n", r);

    int comma = (1, 2, 3);
    printf("(1, 2, 3)    -> %d\n", comma);

    int i = 0;
    int j = i++ || i++;
    printf("j = %d, i = %d\n", j, i);

    int k = 0;
    int m = k++ && k++;
    printf("m = %d, k = %d\n", m, k);

    printf("!5 = %d, !!5 = %d, ~5 = %d, -5 >> 1 = %d\n", !5, !!5, ~5, -5 >> 1);
    return 0;
}
