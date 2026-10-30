/*
 * Lab: A one-slot producer–consumer with a condition variable
 * Topic: Thread
 * Predict before running: In what order do the produce / consume lines appear? Can two produce lines appear in a row?
 * Key idea: pthread_cond_wait atomically releases the mutex and sleeps; when woken it re-acquires the mutex. Always check the condition in a while loop (spurious wakeups).
 * Try next: Change while to if, add a second consumer and use pthread_cond_broadcast — find the scenario where a consumer reads an empty slot.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
static int slot;
static int full = 0;

static void *producer(void *arg)
{
    (void)arg;
    for (int i = 1; i <= 5; i++) {
        pthread_mutex_lock(&m);
        while (full)
            pthread_cond_wait(&cv, &m);
        slot = i * 10;
        full = 1;
        printf("produce %d\n", slot);
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

static void *consumer(void *arg)
{
    (void)arg;
    for (int i = 1; i <= 5; i++) {
        pthread_mutex_lock(&m);
        while (!full)
            pthread_cond_wait(&cv, &m);
        printf("    consume %d\n", slot);
        full = 0;
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

int main(void)
{
    pthread_t p, c;
    pthread_create(&c, NULL, consumer, NULL);
    pthread_create(&p, NULL, producer, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    return 0;
}
