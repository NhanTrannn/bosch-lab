/*
 * Lab: fork() inside if and inside logical expressions
 * Topic: Process
 * Predict before running: How many lines does part [1] print? How many does part [2] print with fork() && fork() || fork()?
 * Key idea: fork() returns 0 in the child and the child's PID (> 0) in the parent. && and || short-circuit: the right side only runs when needed.
 * Try next: Predict the line count of fork() || fork() && fork(); and of (fork() && fork()) || (fork() && fork()).
 * Intentional compiler warning: -Wparentheses and -Wunused-value on the fork() && fork() || fork() line.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

static void reap(void)
{
    while (wait(NULL) > 0)
        ;
}

int main(void)
{
    pid_t root = getpid();

    /* Part 1 */
    if (fork() == 0)
        fork();
    printf("[1] pid = %d\n", getpid());
    reap();
    if (getpid() != root)
        return 0;               /* only the original process continues */

    printf("-----\n");
    fflush(stdout);

    /* Part 2 */
    fork() && fork() || fork();
    printf("[2] pid = %d\n", getpid());
    reap();
    return 0;
}
