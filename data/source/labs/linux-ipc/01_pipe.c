/*
 * Lab: pipe: the parent writes, the child reads
 * Topic: IPC
 * Predict before running: How many bytes does the child read? What does read return the last time? What if the child does NOT close fd[1]?
 * Key idea: pipe(fd): fd[0] is for reading, fd[1] for writing. read returns 0 (EOF) only when EVERY write end is closed — including the write end held by the reading process itself.
 * Try next: Comment out close(fd[1]) in the child branch and run again (Ctrl+C to quit).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(fd[1]);
        char buf[64];
        ssize_t n;
        while ((n = read(fd[0], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            printf("Child read %zd bytes: %s\n", n, buf);
        }
        printf("Child: read returned %zd\n", n);
        close(fd[0]);
        return 0;
    }

    close(fd[0]);
    const char *msg = "hello from parent";
    write(fd[1], msg, strlen(msg));
    close(fd[1]);
    waitpid(pid, NULL, 0);
    printf("Parent: done\n");
    return 0;
}
