/*
 * Lab: stdio buffers and fork
 * Topic: Process
 * Predict before running: What appears on screen? How many times does the letter A appear?
 * Key idea: printf writes into a user-space buffer; fork() duplicates any unflushed buffer too. stdout to a terminal is line-buffered, to a file/pipe fully buffered.
 * Try next: Add fflush(stdout); right after printf("A"). Then change it to printf("A
") and also run ./build/03-process/03_fork_buffer > out.txt; cat out.txt.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    printf("A");
    fork();
    printf("B\n");
    wait(NULL);
    return 0;
}
