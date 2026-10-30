/*
 * Lab: Variables after fork(): copy-on-write
 * Topic: Process
 * Predict before running: What do g, local and heap print in the parent after the child incremented all three? Are the &g addresses the same in parent and child?
 * Key idea: After fork(), the child has its OWN copy of the whole address space (globals, heap, stack). The virtual addresses are identical but map to different physical pages once someone writes (copy-on-write). Sharing data needs IPC (shm, pipes...).
 * Try next: Use mmap(MAP_SHARED | MAP_ANONYMOUS) for the counter and fork — this time the parent sees the child's changes.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int g = 10;

int main(void)
{
    int local = 20;
    int *heap = malloc(sizeof(*heap));
    *heap = 30;

    pid_t pid = fork();
    if (pid == 0) {
        g++;
        local++;
        (*heap)++;
        printf("child:  g=%d local=%d heap=%d | &g=%p heap=%p\n", g, local, *heap, (void *)&g, (void *)heap);
        free(heap);
        exit(0);
    }
    wait(NULL);
    printf("parent: g=%d local=%d heap=%d | &g=%p heap=%p\n", g, local, *heap, (void *)&g, (void *)heap);
    free(heap);
    return 0;
}
