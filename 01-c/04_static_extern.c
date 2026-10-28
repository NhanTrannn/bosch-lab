/*
 * Lab: static locals, file-scope static and extern
 * Topic: C
 * Predict before running: What do n and m print after 3 calls to counter()? If you uncomment the extern int g_hidden line and use it, does it fail at compile time or link time?
 * Key idea: A static local lives for the whole program and is initialized once. File-scope static = internal linkage (other files cannot see it). extern only declares; the definition is in another file.
 * Try next: Remove the static in front of g_hidden in 04_helper.c and rebuild the extern part.
 */
#include <stdio.h>

extern int g_shared;             /* defined in 04_helper.c */
int helper_get_hidden(void);
/* extern int g_hidden; */       /* uncomment and print g_hidden in main */

static int file_counter = 0;

int counter(void)
{
    static int n = 0;
    int m = 0;
    n++;
    m++;
    file_counter++;
    printf("n = %d, m = %d\n", n, m);
    return n;
}

int main(void)
{
    for (int i = 0; i < 3; i++)
        counter();
    printf("file_counter = %d\n", file_counter);
    g_shared += 1;
    printf("g_shared = %d, hidden = %d\n", g_shared, helper_get_hidden());
    return 0;
}
