/*
 * Lab: Thread-specific data (pthread_key) and kinds of mutex: fast, error-checking, recursive, trylock, semaphores (ALP 4.3, 4.4)
 * Topic: ALP chapter 4
 * Predict before running: Whose value does each thread read with pthread_getspecific? When does the key's destructor run? What does a second lock return for an error-checking mutex, and for a recursive one? Another thread unlocking an error-checking mutex? trylock on a mutex someone holds? sem_trywait when the value is 0?
 * Key idea: One key, one value per thread (e.g. a separate log file per thread); the key's destructor runs automatically when a thread ends. Locking a default (fast) mutex twice in the same thread → a permanent deadlock; error-checking returns EDEADLK; recursive counts the locks and needs as many unlocks. pthread_mutex_trylock never blocks and returns EBUSY. Semaphores: sem_wait blocks at 0, sem_trywait returns -1/EAGAIN.
 * Try next: Change the first mutex to PTHREAD_MUTEX_NORMAL and lock it twice (the program hangs; Ctrl+C to quit).
 */
#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static pthread_key_t thread_log_key;

static void close_thread_log(void *value)
{
    printf("  [destructor] closing \"%s\"\n", (char *)value);
    free(value);
}

static void write_to_thread_log(const char *message)
{
    printf("  %s -> writing \"%s\"\n", (char *)pthread_getspecific(thread_log_key), message);
}

static void *thread_function(void *arg)
{
    char *name = malloc(32);
    snprintf(name, 32, "thread%ld.log", (long)arg);
    pthread_setspecific(thread_log_key, name);
    write_to_thread_log("Thread starting.");
    return NULL;
}

static const char *err(int rc)
{
    return rc == 0 ? "0" : rc == EDEADLK ? "EDEADLK" : rc == EPERM ? "EPERM" : rc == EBUSY ? "EBUSY" : strerror(rc);
}

static pthread_mutex_t errcheck;
static void *unlock_other(void *arg)
{
    (void)arg;
    printf("  another thread unlocks errorcheck: %s\n", err(pthread_mutex_unlock(&errcheck)));
    return NULL;
}

static pthread_mutex_t fast = PTHREAD_MUTEX_INITIALIZER;
static void *try_other(void *arg)
{
    (void)arg;
    printf("  another thread trylocks the held mutex: %s\n", err(pthread_mutex_trylock(&fast)));
    return NULL;
}

int main(void)
{
    printf("Thread-specific data:\n");
    pthread_key_create(&thread_log_key, close_thread_log);
    for (long i = 1; i <= 3; ++i) {
        pthread_t t;
        pthread_create(&t, NULL, thread_function, (void *)i);
        pthread_join(t, NULL);
    }
    printf("  main: pthread_getspecific = %s\n", pthread_getspecific(thread_log_key) ? "set" : "NULL");

    printf("Mutex:\n");
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&errcheck, &attr);
    printf("  errorcheck 1st lock: %s\n", err(pthread_mutex_lock(&errcheck)));
    printf("  errorcheck 2nd lock: %s\n", err(pthread_mutex_lock(&errcheck)));
    pthread_t t;
    pthread_create(&t, NULL, unlock_other, NULL);
    pthread_join(t, NULL);
    printf("  errorcheck unlock (by the owner): %s\n", err(pthread_mutex_unlock(&errcheck)));

    pthread_mutex_t recursive;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive, &attr);
    pthread_mutexattr_destroy(&attr);
    int r1 = pthread_mutex_lock(&recursive), r2 = pthread_mutex_lock(&recursive);
    int u1 = pthread_mutex_unlock(&recursive), u2 = pthread_mutex_unlock(&recursive), u3 = pthread_mutex_unlock(&recursive);
    printf("  recursive lock, lock: %s %s; unlock x3: %s %s %s\n", err(r1), err(r2), err(u1), err(u2), err(u3));

    pthread_mutex_lock(&fast);
    printf("  fast: trylock in the thread holding it: %s\n", err(pthread_mutex_trylock(&fast)));
    pthread_create(&t, NULL, try_other, NULL);
    pthread_join(t, NULL);
    pthread_mutex_unlock(&fast);

    printf("Semaphore:\n");
    sem_t sem;
    sem_init(&sem, 0, 2);
    int v;
    sem_getvalue(&sem, &v);
    printf("  initialized to 2, sem_getvalue = %d\n", v);
    sem_wait(&sem);
    sem_wait(&sem);
    errno = 0;
    int rc = sem_trywait(&sem);
    printf("  after 2 sem_waits: sem_trywait = %d, errno = %s\n", rc, errno == EAGAIN ? "EAGAIN" : strerror(errno));
    sem_post(&sem);
    sem_getvalue(&sem, &v);
    printf("  after sem_post: %d\n", v);
    sem_destroy(&sem);
    return 0;
}
