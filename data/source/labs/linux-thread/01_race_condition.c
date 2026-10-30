/*
 * Lab: A race condition with counter++
 * Topic: Thread
 * Predict before running: Does the printed counter equal 2000000? Is it the same over 3 runs?
 * Key idea: counter++ is three steps — read, add, write — and is not atomic. Two interleaving threads lose some increments.
 * Try next: Lower N to 1000 and run it a few times — why is it usually correct now?
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>

#define N 1000000
static long counter = 0;

static void *worker(void *arg)
{
    (void)arg;
    for (int i = 0; i < N; i++)
        counter++;
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    pthread_create(&t1, NULL, worker, NULL);
    pthread_create(&t2, NULL, worker, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("counter = %ld (expected %d)\n", counter, 2 * N);
    return 0;
}
