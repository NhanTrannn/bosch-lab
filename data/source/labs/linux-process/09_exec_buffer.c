/*
 * Lab: exec() replaces the program: the PID stays, the stdio buffer is lost
 * Topic: Process
 * Predict before running: Does the "no newline yet..." text appear? Does the PID change after exec? Is the "never printed" line printed?
 * Key idea: exec does NOT create a new process: it keeps the PID, PPID and open fds, but wipes the old code/data/heap/stack — including an unflushed stdio buffer. exec only returns on failure.
 * Try next: Add fflush(stdout) before execlp. Change "sh" to "no_such_command" to see perror. Run it with > out.txt: the first PID line is lost too (fully buffered).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("PID before exec = %d\n", getpid());
    printf("no newline yet...");
    execlp("sh", "sh", "-c", "echo \"PID after exec  = $$\"", (char *)NULL);
    perror("execlp");
    printf("never printed if exec succeeds\n");
    return 1;
}
