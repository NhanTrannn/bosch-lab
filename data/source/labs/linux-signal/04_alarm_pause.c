/*
 * Lab: alarm, pause and the default action of SIGALRM
 * Topic: Signal
 * Predict before running: What does alarm(0) return? What do pause() and errno return? Does the last line "not printed" appear, and what does the shell report?
 * Key idea: alarm(n) schedules SIGALRM after n seconds (one timer per process; calling it again replaces it and returns the seconds left). pause() sleeps until a signal is handled and always returns -1/EINTR. If SIGALRM is not caught, its default action terminates the process ("Alarm clock").
 * Try next: Use sigaction with SA_RESTART and read() from stdin instead of pause() — is read interrupted?
 */
#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void on_alarm(int sig)
{
    (void)sig;
    const char msg[] = "  [handler] SIGALRM\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(void)
{
    alarm(10);
    unsigned left = alarm(0); /* cancel the old timer */
    printf("alarm(0) returns %u\n", left);

    signal(SIGALRM, on_alarm);
    printf("alarm(1), then pause()...\n");
    alarm(1);
    int r = pause();
    printf("pause() = %d, errno = %s\n", r, strerror(errno));

    signal(SIGALRM, SIG_DFL);
    printf("alarm(1) again, this time WITHOUT catching SIGALRM\n");
    alarm(1);
    pause();
    printf("not printed\n");
    return 0;
}
