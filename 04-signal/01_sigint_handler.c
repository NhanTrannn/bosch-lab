/*
 * !!! Runs until you press Ctrl+C 3 times (or Ctrl+\ to quit immediately).
 * Lab: Catching Ctrl+C with sigaction
 * Topic: Signal
 * Predict before running: What happens on the 1st and 2nd Ctrl+C? On which press does the program exit, and what does the last line print?
 * Key idea: A handler should only set a volatile sig_atomic_t flag and use async-signal-safe functions (write, not printf). The main loop checks the flag and exits cleanly.
 * Try next: While it runs, send kill -INT <PID> from another terminal; then try kill -QUIT <PID> (Ctrl+\).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t stop = 0;
static volatile sig_atomic_t count = 0;

static void on_sigint(int sig)
{
    (void)sig;
    count++;
    if (count >= 3)
        stop = 1;
    const char msg[] = " [handler: got SIGINT]\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    printf("PID %d. Press Ctrl+C 3 times to quit.\n", getpid());
    while (!stop) {
        printf("running... count = %d\n", (int)count);
        sleep(1);
    }
    printf("Clean exit after %d Ctrl+C presses\n", (int)count);
    return 0;
}
