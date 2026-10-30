/*
 * Lab: Reaping zombies with SIGCHLD
 * Topic: Signal
 * Predict before running: Does the program print "Reaped all 5 children"? If the handler called waitpid only once (no while loop), what could go wrong?
 * Key idea: When a child exits, the kernel sends SIGCHLD to the parent. Standard signals are NOT queued: several children exiting close together may trigger the handler only once → loop on waitpid(-1, ..., WNOHANG).
 * Try next: Remove the while loop in the handler (call waitpid once), make the children exit at the same time (remove usleep) and run it a few times.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

static volatile sig_atomic_t reaped = 0;

static void on_sigchld(int sig)
{
    (void)sig;
    int saved = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        reaped++;
    errno = saved;
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    for (int i = 0; i < 5; i++) {
        if (fork() == 0) {
            usleep(100000 * (i + 1));
            _exit(i);
        }
    }

    while (reaped < 5) {
        printf("parent working... reaped %d children\n", (int)reaped);
        sleep(1);
    }
    printf("Reaped all %d children, no zombies left\n", (int)reaped);
    return 0;
}
