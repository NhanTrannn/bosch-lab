/*
 * Lab: fork + exec + waitpid
 * Topic: Process
 * Predict before running: What exit code (WEXITSTATUS) does the parent print? Does the perror("execlp") line run?
 * Key idea: exec replaces the whole process image (keeping the PID); on success it does NOT return. The parent uses waitpid + WIFEXITED/WEXITSTATUS to get the exit code.
 * Try next: Change "/does_not_exist" to "." (exit code 0), then change "ls" to "lsx" (a command that does not exist).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("Child (pid %d) will run ls\n", getpid());
        fflush(stdout);
        execlp("ls", "ls", "-l", "/does_not_exist", (char *)NULL);
        perror("execlp");
        exit(127);
    }

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        printf("Parent: child %d exited with code %d\n", pid, WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("Parent: child %d killed by signal %d\n", pid, WTERMSIG(status));
    return 0;
}
