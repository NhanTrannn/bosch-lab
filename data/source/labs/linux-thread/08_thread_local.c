/*
 * Lab: What threads share and what they own: globals, thread-locals, PID/TID
 * Topic: Thread
 * Predict before running: What mine and shared values does each thread print? What does main see at the end? Is a thread's PID equal to main's?
 * Key idea: Threads of a process share the heap, globals, fds, signal handlers and the PID; each thread has its own stack, registers, TID and _Thread_local variables. On Linux a thread is a task with its own TID (ps -L); getpid() returns the shared TGID.
 * Try next: Add a sleep and look at ps -eLf | grep 08_thread while it runs. Remove _Thread_local and predict again.
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

static int shared = 0;
static _Thread_local int mine = 0;
static pid_t main_pid;

static void *worker(void *arg)
{
    for (int i = 0; i < 3; i++) {
        shared++;
        mine++;
    }
    pid_t tid = (pid_t)syscall(SYS_gettid);
    printf("thread %ld: mine = %d, shared = %d, getpid()==main? %d, tid==pid? %d\n",
           (long)arg, mine, shared, getpid() == main_pid, tid == main_pid);
    return NULL;
}

int main(void)
{
    main_pid = getpid();
    for (long i = 0; i < 2; i++) { /* create then join right away → runs sequentially, fixed output */
        pthread_t t;
        pthread_create(&t, NULL, worker, (void *)i);
        pthread_join(t, NULL);
    }
    pid_t tid = (pid_t)syscall(SYS_gettid);
    printf("main: mine = %d, shared = %d, tid==pid? %d\n", mine, shared, tid == main_pid);
    return 0;
}
