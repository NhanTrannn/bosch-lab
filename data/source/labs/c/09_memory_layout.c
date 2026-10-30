/*
 * Lab: Memory layout: text, data, bss, heap, stack
 * Topic: C
 * Predict before running: Which region does each variable live in (text / rodata / data / bss / heap / stack)? Order the addresses from low to high. What are g_uninit and s_global?
 * Key idea: Code is in text, constants in rodata, initialized globals/statics in data, uninitialized ones in bss (automatically 0), malloc'd memory on the heap (grows up), locals on the stack (near the top, grows down).
 * Try next: Run it twice in a row: do the addresses change? Run size ./build/c/09_memory_layout and cat /proc/self/maps.
 */
#include <stdio.h>
#include <stdlib.h>

int g_init = 10;
int g_uninit;
static int s_global;
const int g_const = 5;

int main(void)
{
    static int s_local = 1;
    int local = 2;
    int *heap = malloc(sizeof(int));
    const char *str = "hello";

    printf("main      : %p\n", (void *)main);
    printf("\"hello\"   : %p\n", (void *)str);
    printf("g_const   : %p\n", (void *)&g_const);
    printf("g_init    : %p\n", (void *)&g_init);
    printf("s_local   : %p\n", (void *)&s_local);
    printf("g_uninit  : %p\n", (void *)&g_uninit);
    printf("s_global  : %p\n", (void *)&s_global);
    printf("heap      : %p\n", (void *)heap);
    printf("local     : %p\n", (void *)&local);

    printf("g_uninit = %d, s_global = %d\n", g_uninit, s_global);
    free(heap);
    return 0;
}
