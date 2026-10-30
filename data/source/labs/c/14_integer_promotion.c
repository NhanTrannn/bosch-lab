/*
 * Lab: Integer promotion: char/short automatically become int
 * Topic: C
 * Predict before running: What is a + b, and c? Is sc == uc? Does the ~x == 0xF0 line print "equal" or "different"? What is sizeof(a + b)?
 * Key idea: In expressions, char/short (signed or unsigned) are promoted to int BEFORE the arithmetic. The result is only truncated when assigned back to a small type. A classic trap with ~ on uint8_t in driver/register code.
 * Intentional compiler warning: -Wsign-compare on the ~x == 0xF0 line (gcc spots exactly this trap).
 * Try next: Change the condition to (uint8_t)~x == 0xF0. Try char c = 200; printf("%d", c); (whether char is signed is implementation-defined; signed on x86, usually unsigned on ARM).
 */
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    unsigned char a = 250, b = 10;
    printf("a + b = %d\n", a + b);
    unsigned char c = a + b;
    printf("c = a + b -> %d\n", c);
    printf("sizeof(a) = %zu, sizeof(a + b) = %zu, sizeof(+a) = %zu\n", sizeof(a), sizeof(a + b), sizeof(+a));

    signed char sc = -1;
    unsigned char uc = 0xFF;
    printf("sc == uc ? %d  (sc -> %d, uc -> %d)\n", sc == uc, sc, uc);

    uint8_t x = 0x0F;
    if (~x == 0xF0)
        printf("equal\n");
    else
        printf("different: ~x = 0x%X\n", ~x);

    uint8_t hi = 0x80;
    printf("hi << 1 = %d, (uint8_t)(hi << 1) = %d\n", hi << 1, (uint8_t)(hi << 1));

    unsigned int un = 0;
    un--;
    printf("un-- from 0 = %u\n", un);

    short s1 = 30000, s2 = 30000;
    int sum = s1 + s2;
    short ssum = s1 + s2;
    printf("int sum = %d, short sum = %d\n", sum, ssum);
    return 0;
}
