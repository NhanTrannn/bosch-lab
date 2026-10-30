/*
 * Lab: Endianness with a pointer and a union
 * Topic: C
 * Predict before running: On an x86_64 PC, what is the first byte of 0x12345678 in memory? In what order does the "Way 2" line print the 4 bytes?
 * Key idea: Little endian stores the least significant byte (LSB) at the lowest address. All members of a union share the same memory.
 * Try next: Write swap32 to flip endianness with shifts and compare it with htonl() from <arpa/inet.h>.
 */
#include <stdio.h>
#include <stdint.h>

union U {
    uint32_t value;
    uint8_t bytes[4];
};

int main(void)
{
    uint32_t x = 0x12345678;
    uint8_t *p = (uint8_t *)&x;
    printf("Way 1: first byte = 0x%02X -> %s endian\n",
           p[0], p[0] == 0x78 ? "little" : "big");

    union U u;
    u.value = 0x12345678;
    printf("Way 2: ");
    for (int i = 0; i < 4; i++)
        printf("%02X ", u.bytes[i]);
    printf("\n");

    union { int i; char c; } t = { .i = 1 };
    printf("Way 3: t.c = %d\n", t.c);
    printf("sizeof(union U) = %zu\n", sizeof(union U));
    return 0;
}
