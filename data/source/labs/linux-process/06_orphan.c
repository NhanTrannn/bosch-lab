/*
 * Lab: Orphan processes
 * Topic: Process
 * Predict before running: How do the PPIDs the child prints on the "before" and "after" lines differ? Where does the shell prompt appear?
 * Key idea: When the parent exits first, the child is adopted by init (PID 1) or a "subreaper" (e.g. systemd --user).
 * Try next: Run ps -o pid,cmd -p <new PPID> to see who adopted the child.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();
    if (pid > 0) {
        printf("Parent %d: exiting after 1s\n", getpid());
        sleep(1);
        return 0;
    }

    printf("Child %d: before, PPID = %d\n", getpid(), getppid());
    sleep(3);
    printf("Child %d: after,  PPID = %d\n", getpid(), getppid());
    return 0;
}
