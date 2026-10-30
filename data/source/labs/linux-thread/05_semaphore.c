/*
 * Lab: A semaphore that limits threads to 2 at a time
 * Topic: Thread
 * Predict before running: What is the "max at the same time" value? Is the enter/leave order fixed?
 * Key idea: A counting semaphore initialized to 2: sem_wait decrements (blocks at 0), sem_post increments. Used to limit how many threads use a resource.
 * Try next: Initialize with sem_init(&sem, 0, 1) (a binary semaphore) and compare it with a mutex.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

static sem_t sem;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static int inside = 0, max_inside = 0;

static void *worker(void *arg)
{
    int id = (int)(long)arg;
    sem_wait(&sem);

    pthread_mutex_lock(&m);
    inside++;
    if (inside > max_inside)
        max_inside = inside;
    printf("T%d enters (inside: %d)\n", id, inside);
    pthread_mutex_unlock(&m);

    usleep(200000);

    pthread_mutex_lock(&m);
    inside--;
    printf("T%d leaves\n", id);
    pthread_mutex_unlock(&m);

    sem_post(&sem);
    return NULL;
}

int main(void)
{
    pthread_t t[5];
    sem_init(&sem, 0, 2);
    for (long i = 0; i < 5; i++)
        pthread_create(&t[i], NULL, worker, (void *)i);
    for (int i = 0; i < 5; i++)
        pthread_join(t[i], NULL);
    printf("Max threads at the same time = %d\n", max_inside);
    sem_destroy(&sem);
    return 0;
}
