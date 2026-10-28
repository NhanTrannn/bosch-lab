# Threads and synchronization

## Theory

### Threads vs processes
| | Process | Thread |
| --- | --- | --- |
| Address space | private | shared within the process |
| Create/destroy, context switch | more expensive | cheaper |
| Communication | needs IPC | read/write shared variables (needs synchronization) |
| A crash (segfault) | only that process dies | the **whole process** dies |

- Threads share: code, data/bss, heap, fds, signal handlers, PID. Private to each thread: stack, registers/PC, TID, signal mask, errno, `thread_local` variables.
- POSIX: `pthread_create`, `pthread_join` (wait + get the return value), `pthread_detach` (cleans up by itself when it ends, can no longer be joined), `pthread_exit`, `pthread_self`. Compile with `-pthread`.
- C++11: `std::thread` (must be joined or detached before it is destroyed, otherwise `std::terminate`), `std::mutex`, `std::lock_guard`, `std::unique_lock`, `std::scoped_lock` (C++17, locks several mutexes without deadlock), `std::condition_variable`, `std::atomic`, `std::async/future`.

### Race conditions and critical sections
- **Race condition**: the result depends on how threads interleave. `counter++` is not atomic: read → add → write.
- A **critical section** solution needs: mutual exclusion, progress, bounded waiting.
- **Atomic**: an operation that cannot be split (`std::atomic<int>`, `__atomic_*`, `stdatomic.h`). Fast for single variables, cannot replace a mutex for multi-step operations.

### Synchronization tools
- **Mutex**: a lock with an owner; only the thread that locked it may unlock it. Waiting threads sleep.
- **Spinlock**: waits in a busy loop without sleeping → suits very short critical sections on multicore, and kernel/interrupt context (where sleeping is not allowed). Useless on a single core.
- **Semaphore**: a counter; `wait/P/sem_wait` decrements (blocks at 0), `post/V/sem_post` increments. A binary semaphore (0/1) resembles a mutex but has **no owner** (another thread can post) → good for signaling between threads/ISRs. A counting semaphore limits access to N resources.
- **Condition variable**: wait for a condition, always used with a mutex; `wait` atomically releases the mutex and sleeps, then re-acquires the mutex on wake-up. Always re-check the condition in a **while loop** (spurious wakeups, several threads woken). `signal/notify_one` wakes one, `broadcast/notify_all` wakes all.
- **Read-write lock**: many readers at once, or a single writer.
- **Recursive mutex**: the same thread can lock it several times.
- **Barrier**: waits until N threads arrive, then lets them all continue.

### Deadlock
The four Coffman conditions (**all four** are needed):
1. Mutual exclusion — the resource cannot be shared.
2. Hold and wait — holding one resource while waiting for another.
3. No preemption — resources cannot be taken away.
4. Circular wait — a cycle of waiting threads.

Prevention: lock in a **fixed order** (breaks circular wait), `trylock` and release on failure, lock everything at once (`std::lock`, `scoped_lock`), timeouts. Avoidance: the Banker's algorithm. **Livelock**: threads keep running but keep yielding to each other and make no progress. **Starvation**: one thread never gets the resource.

### Other concepts
- **Thread-safe**: correct when called concurrently from several threads. **Reentrant**: can be interrupted and called again while still running (e.g. from a signal handler/ISR) — no static/global state, no non-reentrant calls. `strtok` is not reentrant → `strtok_r`.
- Producer–consumer (bounded buffer): mutex + 2 condition variables, or 2 semaphores (empty, full) + a mutex.
- Tools: `valgrind --tool=helgrind`, `-fsanitize=thread`.

## Quiz

### What do threads of the same process NOT share?
- [ ] The heap
- [ ] Global variables
- [x] The stack
- [ ] File descriptors
> Each thread has its own stack, registers and PC.

### Why does `counter++` from two threads without a lock give a wrong result?
- [ ] Because the compiler skips the instruction
- [x] Because it is three steps (read–add–write) and the threads can interleave, losing increments
- [ ] Because the variable is not volatile
- [ ] Because each thread has its own copy of counter
> volatile does not fix it either. Use a mutex or std::atomic.

### Which are the four necessary conditions for deadlock? (select all that apply)
- [x] Mutual exclusion
- [x] Hold and wait
- [x] No preemption
- [x] Circular wait
- [ ] Priority inversion
> Breaking any one of the four prevents deadlock. The most common way: always lock in the same order.

### What is the simplest and most common way to avoid deadlock when two mutexes A and B are needed?
- [ ] Use volatile
- [x] Every thread always locks in the same order (A then B)
- [ ] Raise the thread priority
- [ ] Sleep between the two locks
> Or use std::lock / std::scoped_lock to lock both at once.

### What is the main difference between a mutex and a binary semaphore?
- [ ] There is no difference
- [x] A mutex has an owner (the thread that locks it must unlock it); a semaphore has none, so another thread can post it
- [ ] Semaphores only work between processes
- [ ] A mutex can count above 1
> That is why semaphores suit event signaling (e.g. an ISR notifying a task) and mutexes suit protecting critical sections. Some mutexes support priority inheritance.

### A counting semaphore starts at 3. Four threads call sem_wait in turn (no one posts). What happens?
- [ ] All four pass
- [x] Three threads pass, the fourth blocks
- [ ] Only one thread passes
- [ ] Error
> Each sem_wait decrements by 1; at 0, sem_wait blocks until someone posts.

### Why must `pthread_cond_wait` be called in a `while (!condition)` loop rather than an `if`?
- [ ] To be faster
- [x] Because of spurious wakeups, or another thread may have made the condition false again before this one runs
- [ ] Because cond_wait does not release the mutex
- [ ] POSIX forbids `if`
> Always re-check the condition after waking up.

### What does `pthread_cond_wait(&cond, &mutex)` do with the mutex?
- [ ] Nothing
- [x] Atomically releases the mutex and sleeps; on wake-up it re-acquires the mutex before returning
- [ ] Destroys the mutex
- [ ] Locks it a second time
> You must hold the mutex before calling it.

### When is a spinlock appropriate?
- [ ] Long critical sections with I/O
- [x] Very short critical sections on multicore systems, or contexts that cannot sleep (interrupt handling in the kernel)
- [ ] Single-core systems without preemption
- [ ] Synchronizing two processes over a network
> A spinlock burns CPU while waiting; for long critical sections a mutex (which sleeps) is better.

### `std::thread t(f);` is destroyed without join or detach. What happens?
- [ ] The thread detaches itself
- [ ] The thread is joined automatically
- [x] std::terminate is called and the program aborts
- [ ] Nothing
> C++20's std::jthread joins in its destructor.

### What does `std::lock_guard<std::mutex> lk(m);` give you?
- [ ] It re-locks after every statement
- [x] RAII: the mutex is unlocked automatically when lk leaves scope, even on an exception
- [ ] It lets several threads in at once
- [ ] It is faster than atomic
> unique_lock is more flexible (early unlock, works with condition_variable); scoped_lock locks several mutexes.

### A reentrant function is one that:
- [ ] May only be called by one thread
- [x] Can be interrupted midway and called again (e.g. from an ISR/signal handler) and still be correct
- [ ] Calls itself recursively
- [ ] Always uses a mutex internally
> No unprotected static/global data, no locks held. A function using a mutex can be thread-safe but not reentrant (calling it again from a signal handler deadlocks).

### How is livelock different from deadlock?
- [ ] Livelock only happens on a single core
- [x] The threads stay active (keep changing state) but make no progress
- [ ] Livelock always resolves itself after some time
- [ ] There is no difference
> E.g. two threads both fail trylock, both release, and both retry forever.

### What does a producer–consumer solution with an N-slot buffer and semaphores need?
- [ ] One mutex is enough
- [x] Semaphore empty = N, semaphore full = 0, and a mutex protecting the buffer
- [ ] Semaphore empty = 0, full = N
- [ ] Two mutexes
> Producer: wait(empty) → lock → put → unlock → post(full). Consumer: wait(full) → lock → take → unlock → post(empty).

### `std::atomic<int> x; x++;` from many threads:
- [ ] Still has a race condition
- [x] Each increment is atomic, so the final result is correct
- [ ] Does not compile
- [ ] Needs volatile too
> But a sequence like `if (x == 0) x = 1;` is still not atomic → use compare_exchange or a mutex.

### Which option does gcc need to build a pthread program?
- [ ] -lthread
- [x] -pthread
- [ ] -fthread
- [ ] -D_THREAD
> -pthread both defines the needed macros and links the pthread library (glibc 2.34+ merged it into libc, but you should still use it).
