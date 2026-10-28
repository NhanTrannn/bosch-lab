/*
 * Lab: Passing &i to threads, and getting results back through pthread_join
 * Topic: Thread
 * Predict before running: Which ids does the "WRONG" part print? What does the "RIGHT" part print and what is the sum?
 * Key idea: Every thread gets the SAME address &i while main keeps incrementing i → the threads read a changed value (a data race). The right way: pass the value (cast into void*) or give each thread its own memory. A thread's return value comes back through pthread_join.
 * Try next: Run it many times and with -fsanitize=thread. Return a pointer to a thread's local variable → UB (its stack is gone).
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#define N 4

static void *bad(void *arg)
{
    printf("  WRONG: thread got id = %d\n", *(int *)arg);
    return NULL;
}

static void *good(void *arg)
{
    intptr_t id = (intptr_t)arg;
    return (void *)(id * id);
}

int main(void)
{
    pthread_t t[N];
    for (int i = 0; i < N; i++)
        pthread_create(&t[i], NULL, bad, &i);
    for (int i = 0; i < N; i++)
        pthread_join(t[i], NULL);

    for (intptr_t i = 0; i < N; i++)
        pthread_create(&t[i], NULL, good, (void *)i);
    intptr_t sum = 0;
    for (int i = 0; i < N; i++) {
        void *ret;
        pthread_join(t[i], &ret);
        printf("  RIGHT: thread %d returned %ld\n", i, (long)(intptr_t)ret);
        sum += (intptr_t)ret;
    }
    printf("sum = %ld\n", (long)sum);
    return 0;
}
