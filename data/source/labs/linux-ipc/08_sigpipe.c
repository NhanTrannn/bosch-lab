/*
 * Lab: Writing to a pipe with no reader: SIGPIPE and EPIPE
 * Topic: IPC
 * Predict before running: With SIGPIPE ignored, what does write return? With the default action, how does the child end? What is the default pipe capacity on Linux?
 * Key idea: Writing to a pipe/socket whose read ends are all closed → the kernel sends SIGPIPE (kills the process by default). If SIGPIPE is ignored, write returns -1 with errno = EPIPE. Conversely, reading a pipe whose write ends are all closed → read returns 0 (EOF); forgetting to close a write end → read blocks forever.
 * Try next: Run yes | head -1, then echo ${PIPESTATUS[@]} — yes dies of SIGPIPE (141 = 128 + 13).
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pipe(fd);
    printf("pipe capacity = %d bytes\n", fcntl(fd[1], F_GETPIPE_SZ));
    close(fd[0]); /* nobody reads any more */

    signal(SIGPIPE, SIG_IGN);
    ssize_t n = write(fd[1], "x", 1);
    printf("SIGPIPE ignored: write = %zd, errno = %s\n", n, strerror(errno));

    pid_t pid = fork();
    if (pid == 0) {
        signal(SIGPIPE, SIG_DFL);
        write(fd[1], "x", 1);
        printf("child: not printed\n");
        _exit(0);
    }
    int st;
    waitpid(pid, &st, 0);
    if (WIFSIGNALED(st))
        printf("child killed by signal %d (%s)\n", WTERMSIG(st), strsignal(WTERMSIG(st)));

    int p2[2];
    pipe(p2);
    write(p2[1], "hi", 2);
    close(p2[1]);
    char buf[8];
    ssize_t a = read(p2[0], buf, sizeof(buf));
    ssize_t b = read(p2[0], buf, sizeof(buf));
    printf("1st read = %zd, 2nd read = %zd (EOF)\n", a, b);
    return 0;
}
