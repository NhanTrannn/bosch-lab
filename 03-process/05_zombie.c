/*
 * !!! HANGS ON PURPOSE for ~30 seconds: press Ctrl+C to quit early. Needs 2 terminals.
 * Lab: Zombie processes
 * Topic: Process
 * Predict before running: During those 30 seconds, in a second terminal, what does the STAT column show for the child? How is its command name displayed?
 * Key idea: The child has exited but the parent has not called wait() → the kernel keeps its entry in the process table (PID + exit status) = a zombie.
 * Try next: During the 30 seconds, try kill -9 <child PID>: does the zombie disappear? What happens when you kill the parent?
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child %d: exiting right away\n", getpid());
        exit(0);
    }

    printf("Parent %d: sleeping 30s WITHOUT calling wait(). Child = %d\n", getpid(), pid);
    printf("Terminal 2: ps -o pid,ppid,stat,cmd --ppid %d\n", getpid());
    sleep(30);
    printf("Parent exits\n");
    return 0;
}
