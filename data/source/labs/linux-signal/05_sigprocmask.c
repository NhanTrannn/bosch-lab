/*
 * Lab: Blocking signals (sigprocmask), pending signals, and why signals are not queued
 * Topic: Signal
 * Predict before running: SIGUSR1 is sent 3 times while blocked → after unblocking, how many times does the handler run? And with the real-time signal SIGRTMIN?
 * Key idea: A blocked signal stays pending and is not delivered yet. A standard signal (1–31) is ONE pending bit → sending it several times while blocked delivers it once. Real-time signals (SIGRTMIN..SIGRTMAX) are queued and delivered every time.
 * Try next: Block SIGINT, sleep 5 seconds and press Ctrl+C repeatedly; the program only dies after unblocking. Try blocking SIGKILL — sigprocmask silently ignores it.
 */
#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>

static volatile sig_atomic_t usr1, rt;

static void handler(int sig)
{
    if (sig == SIGUSR1)
        usr1++;
    else
        rt++;
}

int main(void)
{
    signal(SIGUSR1, handler);
    signal(SIGRTMIN, handler);

    sigset_t set, pending;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &set, NULL);

    for (int i = 0; i < 3; i++) {
        raise(SIGUSR1);
        raise(SIGRTMIN);
    }

    sigpending(&pending);
    printf("blocked: SIGUSR1 pending? %d, SIGRTMIN pending? %d\n",
           sigismember(&pending, SIGUSR1), sigismember(&pending, SIGRTMIN));
    printf("before unblocking: usr1 = %d, rt = %d\n", usr1, rt);

    sigprocmask(SIG_UNBLOCK, &set, NULL);
    printf("after unblocking:  usr1 = %d, rt = %d\n", usr1, rt);
    return 0;
}
