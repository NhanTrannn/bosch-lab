/*
 * Lab: Bit manipulation
 * Topic: C
 * Predict before running: Write down the first 4 binary lines and the values on the last 4 lines.
 * Key idea: set: x | (1<<n), clear: x & ~(1<<n), toggle: x ^ (1<<n), read: (x>>n) & 1. x & (x-1) clears the lowest set bit.
 * Try next: Write a function that reverses the bits of a byte and one that swaps its two nibbles.
 */
#include <stdio.h>

unsigned set_bit(unsigned x, int n)    { return x | (1u << n); }
unsigned clear_bit(unsigned x, int n)  { return x & ~(1u << n); }
unsigned toggle_bit(unsigned x, int n) { return x ^ (1u << n); }
int      read_bit(unsigned x, int n)   { return (x >> n) & 1u; }

int count_ones(unsigned x)
{
    int c = 0;
    while (x) {
        x &= x - 1;
        c++;
    }
    return c;
}

int is_pow2(unsigned x) { return x && !(x & (x - 1)); }

void print_bin(unsigned x)
{
    for (int i = 7; i >= 0; i--)
        putchar(read_bit(x, i) ? '1' : '0');
    putchar('\n');
}

int main(void)
{
    unsigned x = 0x2C;
    print_bin(x);
    print_bin(set_bit(x, 0));
    print_bin(clear_bit(x, 3));
    print_bin(toggle_bit(x, 7));
    printf("bit 2 = %d, bit 4 = %d\n", read_bit(x, 2), read_bit(x, 4));
    printf("set bits = %d\n", count_ones(x));
    printf("is_pow2: 0=%d 1=%d 6=%d 64=%d\n", is_pow2(0), is_pow2(1), is_pow2(6), is_pow2(64));
    printf("~x & 0xFF = 0x%X, x >> 2 = %u, x << 1 = %u\n", ~x & 0xFF, x >> 2, x << 1);
    return 0;
}
