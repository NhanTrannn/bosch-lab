/*
 * Lab: Fixing the race with pthread_mutex
 * Topic: Thread
 * Predict before running: What is counter now? Is it slower or faster than lab 01?
 * Key idea: A mutex ensures only one thread at a time is inside the critical section. The price is the lock/unlock overhead.
 * Try next: Time it with time ./build/05-thread/02_mutex_fix; then move the lock outside the for loop and time it again.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>

#define N 1000000
static long counter = 0;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void *worker(void *arg)
{
    (void)arg;
    for (int i = 0; i < N; i++) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
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
    pthread_mutex_destroy(&lock);
    return 0;
}
