/*
 * !!! HANGS ON PURPOSE (infinite loop): press Ctrl+C to quit. Needs 2 terminals.
 * Lab: Sending signals with the kill command
 * Topic: Signal
 * Predict before running: What does the first line print (sigaction for SIGKILL)? For each command kill -TERM, -USR1, -STOP, -CONT, -KILL from terminal 2: how does the program react?
 * Key idea: SIGKILL (9) and SIGSTOP (19) cannot be caught, blocked or ignored. SIGTERM (15) is a "polite" request that can be caught.
 * Try next: Press Ctrl+Z in terminal 1 (SIGTSTP), then type fg. Compare with kill -STOP.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

static void handler(int sig)
{
    const char *msg = sig == SIGTERM ? "[handler] SIGTERM -> still running\n"
                    : sig == SIGUSR1 ? "[handler] SIGUSR1\n"
                    : "[handler] other signal\n";
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGKILL, &sa, NULL) == -1)
        perror("sigaction(SIGKILL)");
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    pid_t me = getpid();
    printf("PID = %d. In terminal 2, try in turn:\n", me);
    printf("  kill -TERM %d ; kill -USR1 %d\n", me, me);
    printf("  kill -STOP %d ; kill -CONT %d ; kill -KILL %d\n", me, me, me);

    for (int i = 0;; i++) {
        printf("tick %d\n", i);
        sleep(2);
    }
}
