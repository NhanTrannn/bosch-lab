---
tags: [linux, alp, threads, mutex, synchronization]
---

# ALP ch.4: Threads

## Theory

Follows *Advanced Linux Programming*, chapter 4 "Threads". Labs: `alp/09`, `10`, `11`; see also `linux-thread/*`. The "Threads and synchronization" topic above gives an overview of race conditions and deadlock.

### Creating threads (4.1)
- A thread runs inside a process and **shares** memory, fds, the environment... with the process's other threads; each thread has its **own stack and registers**. Header `<pthread.h>`, compile/link with `-pthread` (the book uses `-lpthread`).
- `pthread_create(&thread_id, attr, thread_function, arg)`: the thread function has the form `void *f(void *)`. pthread functions return **0 or an error code** (they do not use errno).
- **Passing data**: only one `void *` pointer → pack several parameters into a struct (listing 4.2, `struct char_print_parms`). The memory arg points to must **stay alive** while the thread uses it: the classic bug is passing a struct on main's stack and letting main return before the thread is done.
- **`pthread_join(thread, &ret)`**: waits for the thread to finish and collects its return value (`void *`). Like `wait` for processes. A thread cannot join itself.
- A thread ends when its function returns, when it calls **`pthread_exit(value)`**, or when it is cancelled. A thread calling `exit()` ends the **whole process**. Returning from main also ends the process (every thread).
- `pthread_self()` returns the current thread's ID; compare two IDs with **`pthread_equal`** (not `==`).
- **Attributes** (`pthread_attr_t`): `pthread_attr_init`, `pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)`, `pthread_attr_destroy`. A **joinable** thread (the default) keeps its resources after it ends until it is joined (like a zombie). A **detached** thread cleans up by itself and cannot be joined. `pthread_detach(thread)` makes a thread detached.

### Thread cancellation (4.2)
- `pthread_cancel(thread)` requests cancellation; a cancelled thread returns **`PTHREAD_CANCELED`** to pthread_join. A cancelled thread must still be joined (unless detached).
- Three states:
  - **Asynchronously cancelable**: may be cancelled at any instruction (`pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, ...)`). Dangerous, since it may be cancelled in the middle of an allocation or while holding a lock.
  - **Synchronously cancelable** (deferred, **the default**): the request is held until the thread reaches a **cancellation point** (many blocking system calls such as read, sleep, `pthread_join`, `pthread_cond_wait`, or an explicit `pthread_testcancel()` call).
  - **Uncancelable**: `pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old)`; cancellation requests stay pending until it is re-enabled. Used to protect a **critical section** that must not stop halfway (listing 4.6: transferring money between two accounts).
- The book's advice: avoid cancelling threads when possible; instead tell the thread to exit by itself.

### Thread-specific data (4.3)
- **Thread-specific data**: each thread has its own copy of a "global variable". `pthread_key_create(&key, destructor)` creates the key (once), `pthread_setspecific(key, value)` and `pthread_getspecific(key)` set/get the calling thread's value. A thread that set nothing gets NULL. Listing 4.7: each thread logs to its own file `thread<id>.log`.
- The key's **destructor** runs automatically when a thread ends (for non-NULL values), even if the thread was cancelled.
- (C11/GCC also offer the simpler `__thread`/`_Thread_local`; see lab `linux-thread/08_thread_local`.)
- **Cleanup handlers**: `pthread_cleanup_push(func, arg)` and `pthread_cleanup_pop(execute)` must be **paired in the same block** (they are macros). The handler runs if the thread is cancelled or calls pthread_exit between push and pop. `pop(1)` removes and runs the handler; `pop(0)` only removes it. Used to free memory or unlock a mutex on cancellation.
- **C++**: `pthread_exit` does not guarantee that destructors of local objects run. The book recommends throwing a special exception, catching it in the outermost thread function and only then calling pthread_exit, so the stack unwinds properly (listing 4.9).

### Synchronization and critical sections (4.4)
- **Race condition**: the result depends on the order threads run in. The book's example is a job queue where two threads take the same job, or one thread takes a job another thread just freed.
- **Mutex** (MUTual EXclusion): `pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;` or `pthread_mutex_init(&m, &attr)`. `pthread_mutex_lock` (blocks if already locked), `pthread_mutex_unlock`. Keep critical sections as short as possible.
- The book's three kinds of mutex:
  - **fast** (the default): a thread locking a mutex it already holds → **deadlock** forever.
  - **recursive**: may be locked several times, counts the locks; must be unlocked as many times.
  - **error-checking**: locking again → returns `EDEADLK`; unlocking a mutex you do not own → `EPERM`.
  - Set with `pthread_mutexattr_settype` (the book uses the GNU names `pthread_mutexattr_setkind_np`, `PTHREAD_MUTEX_RECURSIVE_NP`, `PTHREAD_MUTEX_ERRORCHECK_NP`).
- **`pthread_mutex_trylock`**: does not block; a held mutex → returns `EBUSY` so the thread can do something else and retry.
- **Semaphores for threads** (`<semaphore.h>`): a non-negative counter. `sem_init(&s, 0, value)` (0 = shared only between threads of the process), `sem_wait` (decrement; blocks at 0), `sem_post` (increment, wakes a waiter), `sem_trywait` (non-blocking, returns EAGAIN), `sem_getvalue`, `sem_destroy`. The book's job queue: a mutex protects the list and a semaphore counts the jobs, so idle threads block instead of spinning.
- **Condition variables**: wait for a condition without burning CPU. `pthread_cond_wait(&cond, &mutex)` **releases the mutex and sleeps in one atomic step**; when woken it re-acquires the mutex before returning. `pthread_cond_signal` wakes one thread, `pthread_cond_broadcast` wakes all. Always re-check the condition in a **while loop** after waking. The thread setting the condition must hold the mutex while changing the flag and signaling (listing 4.14, `thread_flag`).
- **Deadlock with two threads**: each holds one lock and waits for the other's. Prevention: every thread takes the locks in **the same order**. Deadlock can also happen between a process and its signal handler.

### GNU/Linux thread implementation (4.5)
- The book (2001, the **LinuxThreads** library): each thread is a **separate process** created with the **`clone`** system call, sharing the address space. So each thread has a different PID (`getpid` differs, `ps` shows several lines), plus an extra "manager thread". Signals from outside usually go to the main thread's process; to signal one specific thread use **`pthread_kill(thread, sig)`**.
- Today (**NPTL**, since kernel 2.6): all threads of a process share **the same PID** (the thread group ID); each thread has its own TID (`gettid()`, `ps -L`, `/proc/PID/task/`). There is no manager thread. A signal sent to the process is delivered to any one thread that does not block it. `clone` is still used underneath.
- `clone` is the general form of fork and pthread_create: it chooses which resources are shared (memory, fds, signal handlers...). Ordinary programs should not call it directly.

### Processes vs. threads (4.6)
- All threads of a program must run **the same executable**; a child process can exec a different program.
- A misbehaving thread (a bad pointer) can **crash the whole process**; a misbehaving child process does not affect other processes → processes are safer.
- Creating a thread is faster than fork (no page tables to copy...). Sharing data between threads is easier but needs synchronization; processes need IPC.
- Threads suit splitting one task into parallel parts that share a lot of data; processes suit independent tasks, fault isolation, or running other programs.

## Quiz

### Which signature must the thread function passed to pthread_create have? {#alp-ch4-001}
- [ ] `void f(void)`
- [ ] `int f(int)`
- [x] `void *f(void *)`
- [ ] `void *f(int, char **)`
> It takes a single `void *` and returns a `void *`. Pack several parameters into a struct.

### How does pthread_create report an error? {#alp-ch4-002}
- [ ] Returns -1 and sets errno
- [x] Returns a non-zero error code (e.g. EAGAIN); errno is not set
- [ ] Returns NULL
- [ ] Raises SIGABRT
> Most pthread_* functions work this way. Semaphores (sem_*) still use -1 and errno.

### What is wrong with this code? {#alp-ch4-003}
```c
void *print(void *arg) { struct parms *p = arg; /* use p */ return NULL; }
int main() {
    pthread_t t;
    struct parms p = { 'x', 30000 };
    pthread_create(&t, NULL, print, &p);
    return 0;
}
```
- [ ] Nothing
- [x] main returns without joining: the process ends (the thread may not have finished) and the struct p on main's stack dies while the thread may still use it
- [ ] Structs cannot be passed through void*
- [ ] print must call pthread_exit
> Listing 4.2 in the book has exactly this bug; listing 4.3 fixes it by calling pthread_join before returning.

### How do you get a thread's return value? {#alp-ch4-004}
- [ ] Read the global variable `pthread_retval`
- [x] The second argument of `pthread_join(thread, &ret)`
- [ ] The return value of pthread_create
- [ ] `pthread_self()`
> The thread returns with `return ptr;` or `pthread_exit(ptr)`. Never return a pointer to a local variable of the thread.

### A thread (not main) calls `exit(0)`. What happens? {#alp-ch4-005}
- [ ] Only that thread ends
- [x] The whole process (every thread) ends
- [ ] Compile error
- [ ] The thread becomes a zombie
> To end only the current thread, use pthread_exit or return from the thread function.

### Why compare thread IDs with pthread_equal instead of `==`? {#alp-ch4-006}
- [ ] pthread_equal is faster
- [x] pthread_t is an opaque type and may be a struct on some systems, so `==` may not work
- [ ] `==` always returns false
- [ ] pthread_t is a string
> On Linux pthread_t is an `unsigned long`, but portable code must use pthread_equal.

### How does a detached thread differ from a joinable one? {#alp-ch4-007}
- [ ] Detached threads are faster
- [x] A detached thread frees its resources by itself when it ends and cannot be joined; a joinable thread keeps them until it is joined
- [ ] A detached thread does not share memory with the process
- [ ] A detached thread cannot be cancelled
> Create it with the `PTHREAD_CREATE_DETACHED` attribute or call `pthread_detach`. A joinable thread nobody joins is like a zombie.

### What is the default cancellation state of a newly created thread? {#alp-ch4-008}
- [ ] Asynchronously cancelable
- [x] Synchronously (deferred) cancelable: cancelled only at a cancellation point
- [ ] Uncancelable
- [ ] Cannot be cancelled
> Change it with `pthread_setcanceltype` (async/deferred) and `pthread_setcancelstate` (enable/disable).

### Which function is an explicit cancellation point, used by a thread to check for cancellation inside a long computation loop? {#alp-ch4-009}
- [ ] `pthread_yield`
- [x] `pthread_testcancel`
- [ ] `pthread_cancel`
- [ ] `sched_yield`
> A pure computation loop with no system calls has no cancellation points, so a deferred cancellation request would never take effect without pthread_testcancel.

### A thread inside a `PTHREAD_CANCEL_DISABLE` region receives pthread_cancel. What happens? {#alp-ch4-010}
- [ ] It is cancelled immediately
- [ ] The request is ignored forever
- [x] The request stays pending; once the thread re-enables cancellation and reaches a cancellation point, it is cancelled
- [ ] pthread_cancel returns EBUSY
> This is how you protect a critical section that must complete (listing 4.6: a money transfer).

### What does pthread_join return as the value of a cancelled thread? {#alp-ch4-011}
- [ ] NULL
- [ ] -1
- [x] `PTHREAD_CANCELED`
- [ ] The last value the thread intended to return
> PTHREAD_CANCELED is a special pointer constant, usually `(void *)-1`.

### Which problem does thread-specific data solve? {#alp-ch4-012}
- [ ] Fast data sharing between threads
- [x] Each thread gets its own copy of the same "global variable" (e.g. its own log file), accessed through the same key
- [ ] Protecting data from race conditions
- [ ] Passing data from a thread back to main
> pthread_key_create once, then each thread calls pthread_setspecific with its own value. pthread_getspecific returns the calling thread's value.

### When is the destructor passed to `pthread_key_create` called? {#alp-ch4-013}
- [ ] When pthread_key_delete is called
- [x] When each thread ends, if that thread's value for the key is not NULL
- [ ] When the process ends
- [ ] When pthread_setspecific is called a second time
> The destructor also runs when the thread is cancelled or calls pthread_exit. Used to close files or free the thread's own memory.

### Which statements about pthread_cleanup_push/pop are TRUE? (select all that apply) {#alp-ch4-014}
- [x] They must be used as a pair in the same block
- [x] The handler runs if the thread is cancelled between push and pop
- [x] `pthread_cleanup_pop(1)` both removes and runs the handler
- [ ] The handler runs if the thread returns normally from between push and pop
> push/pop are macros that open and close braces. Returning in between is undefined behavior; pthread_exit does run the handler.

### In C++, why does the book advise against calling pthread_exit directly in the middle of a thread function? {#alp-ch4-015}
- [ ] pthread_exit does not exist in C++
- [x] Destructors of local objects on the thread's stack may not run; instead throw an exception and catch it in the outermost thread function
- [ ] pthread_exit ends the whole process
- [ ] pthread_exit cannot return a value
> Listing 4.9: a `ThreadExitException` class is thrown to exit; the thread function catches it and then calls pthread_exit. The stack unwinds and destructors (RAII) run.

### A thread locks a default (fast) mutex that it already holds. What happens? {#alp-ch4-016}
- [ ] It returns EDEADLK
- [ ] The lock succeeds and a counter goes up
- [x] Deadlock: the thread blocks forever
- [ ] It returns EBUSY
> An error-checking mutex returns EDEADLK; a recursive mutex allows the relock.

### Which kind of mutex lets the same thread lock it several times? {#alp-ch4-017}
- [ ] fast
- [x] recursive
- [ ] error-checking
- [ ] None
> Each lock needs a matching unlock; the mutex is only really released when the count reaches 0.

### What does `pthread_mutex_trylock` return when another thread holds the mutex? {#alp-ch4-018}
- [ ] It blocks until it gets the mutex
- [x] EBUSY, immediately
- [ ] EAGAIN, setting errno
- [ ] 0
> Used when the thread has other work to do instead of waiting.

### In `sem_init(&s, 0, 3)`, what do the second argument 0 and third argument 3 mean? {#alp-ch4-019}
- [ ] Shared between processes, maximum value 3
- [x] Shared only between threads of the process, initial value 3
- [ ] A binary semaphore with 3 waiting threads
- [ ] 0-second timeout, 3 retries
> A non-zero pshared makes the semaphore usable between processes; it must then live in shared memory.

### A thread calls `sem_wait` while the semaphore's value is 0. What happens? {#alp-ch4-020}
- [ ] The value becomes -1
- [x] The thread blocks until another thread calls sem_post
- [ ] It returns an error immediately
- [ ] The semaphore is reset to its initial value
> A semaphore's value is never negative. `sem_trywait` does not block; it returns -1 with errno = EAGAIN.

### In the book's job queue, why is a semaphore counting jobs used in addition to the mutex? {#alp-ch4-021}
- [ ] To replace the mutex
- [x] So idle threads block while the queue is empty instead of repeatedly locking the mutex to check (busy waiting), and wake exactly when a new job arrives
- [ ] To limit the number of threads
- [ ] To speed up malloc
> The mutex protects the list; the semaphore counts the elements, so a successful sem_wait guarantees there is a job to take.

### What does `pthread_cond_wait(&cond, &mutex)` do? {#alp-ch4-022}
- [ ] Waits for the mutex to be released
- [x] Releases the mutex and sleeps in one atomic step; when woken it re-acquires the mutex before returning
- [ ] Wakes a waiting thread
- [ ] Locks the mutex and waits while still holding it
> The atomic step avoids lost wake-ups: there is no gap between checking the condition and starting to sleep. You must hold the mutex when calling it.

### Why must pthread_cond_wait be placed in a `while (!flag)` loop rather than an `if`? {#alp-ch4-023}
- [ ] The syntax requires it
- [x] Because of spurious wakeups, or another thread may change the condition before this one re-acquires the mutex
- [ ] To wait longer
- [ ] cond_wait can only wait once
> Always re-check the condition after waking.

### How do `pthread_cond_signal` and `pthread_cond_broadcast` differ? {#alp-ch4-024}
- [ ] signal is for processes, broadcast for threads
- [x] signal wakes one waiting thread; broadcast wakes all of them
- [ ] broadcast releases the mutex, signal does not
- [ ] No difference
> If no thread is waiting, neither does anything (the signal is not remembered, unlike a semaphore).

### Thread 1 locks A then B, thread 2 locks B then A. What is the risk and the fix? {#alp-ch4-025}
- [ ] A race condition; fix with volatile
- [x] Deadlock; fix by having every thread take the locks in the same order (A then B)
- [ ] Starvation; fix with nice
- [ ] No risk
> Each thread holds one lock and waits for the other. trylock-and-release-everything-then-retry also works.

### According to the book (LinuxThreads), what does getpid() return in different threads of the same program? {#alp-ch4-026}
- [ ] Always the same value
- [x] A different PID in each thread, because each thread is a process created with clone
- [ ] The thread ID
- [ ] An error, because getpid is not thread-safe
> That was Linux in 2001. With today's NPTL every thread has the same PID (the thread group ID) and its own TID (`gettid()`, shown by `ps -L`).

### Which system call does Linux use to create threads, as a general form of fork that chooses which resources are shared? {#alp-ch4-027}
- [ ] `vfork`
- [x] `clone`
- [ ] `spawn`
- [ ] `exec`
> E.g. the flags CLONE_VM (shared memory), CLONE_FILES (shared fd table), CLONE_SIGHAND, CLONE_THREAD. The book advises against calling clone directly.

### Which function sends a signal to a specific thread in the same program? {#alp-ch4-028}
- [ ] `kill`
- [x] `pthread_kill`
- [ ] `raise`
- [ ] `sigqueue`
> `pthread_kill(thread_id, SIGUSR1)`. kill() targets the whole process.

### When should you use several processes rather than several threads? (select all that apply) {#alp-ch4-029}
- [x] When you need to run a different program (executable)
- [x] When you need fault isolation: one broken part must not bring down the rest
- [ ] When large data structures are constantly shared
- [ ] When execution units must be created and destroyed very quickly, very often
> Heavy data sharing and fast creation/destruction favor threads.
