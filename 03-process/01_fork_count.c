/*
 * Lab: fork() three times
 * Topic: Process
 * Predict before running: How many "PID = ..." lines are printed? How many different PPIDs are there?
 * Key idea: Every running process executes the next fork() → the number of processes doubles after each fork: n sequential forks → 2^n processes.
 * Try next: Replace the 3 forks with the loop for (i = 0; i < 3; i++) fork(); — is the result different? And for (i < 4)?
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    fork();
    fork();
    fork();
    printf("PID = %d, PPID = %d\n", getpid(), getppid());

    while (wait(NULL) > 0)      /* wait for all children so the output stays tidy */
        ;
    return 0;
}
