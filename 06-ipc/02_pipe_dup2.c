/*
 * Lab: Implementing ls / | wc -l with dup2
 * Topic: IPC
 * Predict before running: What number does the program print (compared with running ls / | wc -l in the shell)? What if the parent forgets to close fd[1]?
 * Key idea: dup2(old, new) makes new refer to the same file as old. A shell implements a pipeline as: pipe → fork twice → dup2 stdout/stdin → exec.
 * Try next: Comment out the parent's close(fd[0]); close(fd[1]);. Then extend it to ls / | sort -r | head -3.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pipe(fd);

    pid_t p1 = fork();
    if (p1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("ls", "ls", "/", (char *)NULL);
        perror("exec ls");
        _exit(1);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("wc", "wc", "-l", (char *)NULL);
        perror("exec wc");
        _exit(1);
    }

    close(fd[0]);
    close(fd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    printf("Done: same as  ls / | wc -l\n");
    return 0;
}
