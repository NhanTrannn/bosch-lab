/*
 * Lab: Thread cancellation: deferred cancellation, cancellation points, uncancelable sections, cleanup handlers (ALP 4.2, 4.3.1)
 * Topic: ALP chapter 4
 * Predict before running: Thread A is cancelled while sleeping: does its cleanup handler run, and what does join return? Thread B disables cancellation during a "money transfer": where is it cancelled, and is "transfer done" printed? What does pthread_cleanup_pop(1) do when the thread is not cancelled?
 * Key idea: Threads are deferred (synchronously) cancelable by default: a cancellation request only takes effect at a cancellation point (sleep, read, pthread_join, pthread_testcancel...). PTHREAD_CANCEL_DISABLE creates an uncancelable critical section; the request is kept until cancellation is re-enabled. On cancellation, cleanup handlers (pthread_cleanup_push/pop) run to release resources. A cancelled thread returns PTHREAD_CANCELED.
 * Try next: Switch thread A to PTHREAD_CANCEL_ASYNCHRONOUS and replace the sleep with a busy loop. Remove the pthread_testcancel in thread B.
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void deallocate_buffer(void *buffer)
{
    printf("  [cleanup] free buffer\n");
    fflush(stdout);
    free(buffer);
}

static volatile int a_ready = 0, b_ready = 0;

static void *thread_a(void *arg)
{
    (void)arg;
    void *temp_buffer = malloc(1024);
    pthread_cleanup_push(deallocate_buffer, temp_buffer);
    a_ready = 1;
    for (int i = 0; i < 100; ++i) usleep(100000);   /* usleep is a cancellation point */
    printf("  [A] never reached\n");
    pthread_cleanup_pop(1);
    return NULL;
}

static void *thread_b(void *arg)
{
    (void)arg;
    int old_state;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
    printf("  [B] transfer started (cancellation disabled)\n");
    fflush(stdout);
    b_ready = 1;
    usleep(300000);   /* a cancellation point, but cancellation is disabled */
    printf("  [B] transfer done\n");
    fflush(stdout);
    pthread_setcancelstate(old_state, NULL);
    pthread_testcancel();
    printf("  [B] never reached\n");
    return NULL;
}

static void *thread_c(void *arg)
{
    (void)arg;
    void *buf = malloc(16);
    pthread_cleanup_push(deallocate_buffer, buf);
    printf("  [C] finished normally, pop(1) still runs the handler:\n");
    fflush(stdout);
    pthread_cleanup_pop(1);
    return (void *)42;
}

int main(void)
{
    pthread_t a, b, c;
    void *ret;

    printf("Thread A:\n");
    pthread_create(&a, NULL, thread_a, NULL);
    while (!a_ready) usleep(1000);
    usleep(50000);
    pthread_cancel(a);
    pthread_join(a, &ret);
    printf("  join A: %s\n", ret == PTHREAD_CANCELED ? "PTHREAD_CANCELED" : "another value");

    printf("Thread B:\n");
    fflush(stdout);
    pthread_create(&b, NULL, thread_b, NULL);
    while (!b_ready) usleep(1000);
    printf("  [main] pthread_cancel(B) = %d\n", pthread_cancel(b));
    fflush(stdout);
    pthread_join(b, &ret);
    printf("  join B: %s\n", ret == PTHREAD_CANCELED ? "PTHREAD_CANCELED" : "another value");

    printf("Thread C:\n");
    fflush(stdout);
    pthread_create(&c, NULL, thread_c, NULL);
    pthread_join(c, &ret);
    printf("  join C: %ld\n", (long)ret);
    return 0;
}
