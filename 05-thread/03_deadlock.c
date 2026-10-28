/*
 * !!! HANGS ON PURPOSE: the program will freeze; press Ctrl+C to quit.
 * Lab: Deadlock from locking in opposite orders
 * Topic: Thread
 * Predict before running: Which lines does the program print, and then what? Does the "No deadlock" line appear?
 * Key idea: T1 holds A and waits for B, T2 holds B and waits for A → a circular wait forever. The most common fix: every thread locks the mutexes in the SAME order.
 * Try next: Make T2 lock A before B. Or use pthread_mutex_trylock and release A if B cannot be taken.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t A = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t B = PTHREAD_MUTEX_INITIALIZER;

static void *t1(void *arg)
{
    (void)arg;
    pthread_mutex_lock(&A);
    printf("T1: holding A, waiting for B...\n");
    usleep(100000);
    pthread_mutex_lock(&B);
    printf("T1: has both A and B\n");
    pthread_mutex_unlock(&B);
    pthread_mutex_unlock(&A);
    return NULL;
}

static void *t2(void *arg)
{
    (void)arg;
    pthread_mutex_lock(&B);
    printf("T2: holding B, waiting for A...\n");
    usleep(100000);
    pthread_mutex_lock(&A);
    printf("T2: has both A and B\n");
    pthread_mutex_unlock(&A);
    pthread_mutex_unlock(&B);
    return NULL;
}

int main(void)
{
    pthread_t x, y;
    pthread_create(&x, NULL, t1, NULL);
    pthread_create(&y, NULL, t2, NULL);
    pthread_join(x, NULL);
    pthread_join(y, NULL);
    printf("No deadlock!\n");
    return 0;
}
