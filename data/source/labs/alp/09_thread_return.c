/*
 * Lab: pthreads: passing data to a thread, return values through join, pthread_exit, detached threads (ALP 4.1)
 * Topic: ALP chapter 4
 * Predict before running: What is the 1000th prime computed by the thread? What does pthread_equal(self, main) give inside the child thread? What happens when you pthread_join a detached thread? Is it safe for two character-printing threads to use structs on main's stack?
 * Key idea: pthread_create(&id, attr, function, void *arg) can pass only ONE pointer → pack several parameters into a struct. The memory arg points to must stay alive until the thread is done with it (that is why main must join before returning). The return value is a void*, collected with pthread_join (return or pthread_exit both work). A detached thread frees itself when it ends and cannot be joined.
 * Try next: Remove the pthread_join calls for char_print and run it a few times. Build without -pthread to see the link error.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Listing 4.2, with main fixed as in listing 4.3 */
struct char_print_parms { char character; int count; };

static void *char_print(void *parameters)
{
    struct char_print_parms *p = parameters;
    for (int i = 0; i < p->count; ++i) fputc(p->character, stderr);
    return NULL;
}

/* Listing 4.4: compute the nth prime (slowly), returned through void* */
static void *compute_prime(void *arg)
{
    int candidate = 2;
    int n = *((int *)arg);
    while (1) {
        int factor, is_prime = 1;
        for (factor = 2; factor < candidate; ++factor)
            if (candidate % factor == 0) { is_prime = 0; break; }
        if (is_prime && --n == 0)
            return (void *)(intptr_t)candidate;
        ++candidate;
    }
}

static pthread_t main_thread;
static void *who_am_i(void *arg)
{
    (void)arg;
    printf("in the child thread: pthread_equal(self, main) = %d\n", pthread_equal(pthread_self(), main_thread) != 0);
    pthread_exit((void *)(intptr_t)7);
}

static sem_t release;
static void *detached_worker(void *arg)
{
    (void)arg;
    sem_wait(&release);
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    struct char_print_parms a = {'x', 30}, b = {'o', 20};
    pthread_create(&t1, NULL, char_print, &a);
    pthread_create(&t2, NULL, char_print, &b);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    fprintf(stderr, "\n(30 x and 20 o; the interleaving is not fixed)\n");

    int which = 1000;
    void *prime;
    pthread_t pt;
    pthread_create(&pt, NULL, compute_prime, &which);
    pthread_join(pt, &prime);
    printf("prime number %d = %d\n", which, (int)(intptr_t)prime);

    main_thread = pthread_self();
    void *ret;
    pthread_t w;
    pthread_create(&w, NULL, who_am_i, NULL);
    pthread_join(w, &ret);
    printf("pthread_exit((void*)7) -> join got %d\n", (int)(intptr_t)ret);

    sem_init(&release, 0, 0);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t d;
    pthread_create(&d, &attr, detached_worker, NULL);
    pthread_attr_destroy(&attr);
    int rc = pthread_join(d, NULL);
    printf("pthread_join on a detached thread: %d (%s)\n", rc, rc == EINVAL ? "EINVAL" : strerror(rc));
    sem_post(&release);
    return 0;
}
