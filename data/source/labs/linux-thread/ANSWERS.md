# Linux: threads — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

### 01_race_condition — A race condition with counter++ ⚠️ nondeterministic

**Question:** Does the printed counter equal 2000000? Is it the same over 3 runs?

**Expected output:**

```text
counter = 1087342 (expected 2000000)     <- less than 2000000, different on every run
```

**Explanation:**

- Thread 1 reads counter = 5, thread 2 also reads 5, both write 6 → one increment is lost (a lost update).
- The result is **nondeterministic**; on a single-core machine or with a small N it may happen to be right — the race condition is still there.
- `volatile` does **not** fix it (it only prevents optimizations; it does not make anything atomic).

**Exam relevance:** Three fixes: a mutex, an atomic variable (`__atomic_fetch_add`, `stdatomic.h`, `std::atomic`), or let each thread count separately and add up after joining.

### 02_mutex_fix — Fixing the race with pthread_mutex

**Question:** What is counter now? Is it slower or faster than lab 01?

**Expected output:**

```text
counter = 2000000 (expected 2000000)
```

**Explanation:**

- Always exactly 2000000. It runs noticeably **slower** because of 2 million lock/unlock pairs and contention between CPU cores.
- Linux implements mutexes with **futexes**: an uncontended lock stays entirely in user space; only contention enters the kernel.
- Hold locks as briefly as possible, but locking in tiny pieces also has a cost.

**Exam relevance:** A mutex has **ownership**: the thread that locks it unlocks it. Locking a normal (default) mutex twice in one thread → deadlock; use a `RECURSIVE` mutex if you need that.

### 03_deadlock — Deadlock from locking in opposite orders

**Question:** Which lines does the program print, and then what? Does the "No deadlock" line appear?

**Expected output:**

```text
T1: holding A, waiting for B...
T2: holding B, waiting for A...
(frozen forever — Ctrl+C to quit; the two lines may appear in either order)
```

**Explanation:**

- All 4 Coffman conditions hold: **mutual exclusion**, **hold and wait**, **no preemption**, **circular wait**.
- The `usleep` only makes the deadlock near-certain; without it the program can still deadlock, just occasionally.
- Debugging: `gdb -p <PID>`, then `thread apply all bt` shows both threads stuck in `__lll_lock_wait`.
- Fixes: lock in a fixed order (breaks the circular wait), `trylock` + back-off, or `std::scoped_lock(a, b)` in C++17.

**Exam relevance:** Livelock: threads keep running but keep yielding to each other and make no progress. Starvation: a thread never gets the resource.

### 04_cond_var — A one-slot producer–consumer with a condition variable

**Question:** In what order do the produce / consume lines appear? Can two produce lines appear in a row?

**Expected output:**

```text
produce 10
    consume 10
produce 20
    consume 20
produce 30
    consume 30
produce 40
    consume 40
produce 50
    consume 50
```

**Explanation:**

- The slot holds 1 item: the producer must wait for `full == 0`, the consumer for `full == 1` → strict alternation; two produce lines in a row are **impossible**.
- `pthread_cond_signal` has no "memory": if nobody is waiting, the signal is lost. So the real state lives in the `full` variable, protected by the mutex.
- `while` is needed because of spurious wakeups, or because another thread may already have taken the data.

**Exam relevance:** A condition variable always goes with 1 mutex + 1 condition (predicate). `signal` wakes at least 1 thread, `broadcast` wakes them all.

### 05_semaphore — A semaphore that limits threads to 2 at a time ⚠️ nondeterministic

**Question:** What is the "max at the same time" value? Is the enter/leave order fixed?

**Expected output:**

```text
(the order varies, e.g.)
T0 enters (inside: 1)
T1 enters (inside: 2)
T0 leaves
T2 enters (inside: 2)
T1 leaves
T3 enters (inside: 2)
...
Max threads at the same time = 2
```

**Explanation:**

- Never more than 2 threads are between `sem_wait` and `sem_post` → max = 2.
- The separate mutex `m` only protects the `inside` counter and the `printf`.
- The 2nd argument of `sem_init` = 0: shared between threads; ≠ 0: shared between processes (the semaphore must then live in shared memory).

**Exam relevance:** Mutex vs semaphore: a mutex has an owner and is for mutual exclusion; a semaphore is a counter that another thread can `post`, used for signaling/limiting resources. Named semaphores: `sem_open("/name", ...)`.

### 06_cpp_thread — std::thread, std::mutex, lock_guard, std::atomic ⚠️ nondeterministic

**Question:** What are plain, guarded and atomic? Which one can be wrong?

**Expected output:**

```text
plain   = 1203117      <- wrong, changes on every run
guarded = 2000000
atomic  = 2000000
CPU cores = 8          <- depends on the machine
```

**Explanation:**

- `plain` is the same race condition as lab 01.
- `lock_guard` locks in its constructor and unlocks in its destructor → you cannot forget to unlock, even with exceptions.
- `atomic<int>` uses atomic CPU instructions (x86: `lock xadd`), usually faster than a mutex for simple operations.
- A `std::thread` that is still **joinable** when destroyed → `std::terminate()` → the program aborts.

**Exam relevance:** `unique_lock` is more flexible than `lock_guard` (unlock midway, works with `condition_variable`). C++20's `std::jthread` joins automatically when destroyed.

### 07_thread_arg_bug — Passing &i to threads, and getting results back through pthread_join ⚠️ nondeterministic

**Question:** Which ids does the "WRONG" part print? What does the "RIGHT" part print and what is the sum?

**Expected output:**

```text
  WRONG: thread got id = 2
  WRONG: thread got id = 3
  WRONG: thread got id = 4
  WRONG: thread got id = 2
  RIGHT: thread 0 returned 0
  RIGHT: thread 1 returned 1
  RIGHT: thread 2 returned 4
  RIGHT: thread 3 returned 9
sum = 14
```

**Explanation:**

- The WRONG part: each thread reads `*arg` some time **after** it was created; by then main may already have incremented i, or even left the loop (value 4, or the memory reused by the next loop). Every run differs, and the line order changes too.
- The RIGHT part: the id is passed **by value** inside the pointer itself → each thread has its own copy. Results come back through `pthread_join(t, &ret)` in join order → fixed: 0 + 1 + 4 + 9 = 14.

**Exam relevance:** Passing `&i` to threads is a favorite "find the bug" question. A thread returning a pointer to its own local variable is wrong (its stack is gone) — return malloc'd memory or a value cast to a pointer.

### 08_thread_local — What threads share and what they own: globals, thread-locals, PID/TID

**Question:** What mine and shared values does each thread print? What does main see at the end? Is a thread's PID equal to main's?

**Expected output:**

```text
thread 0: mine = 3, shared = 3, getpid()==main? 1, tid==pid? 0
thread 1: mine = 3, shared = 6, getpid()==main? 1, tid==pid? 0
main: mine = 0, shared = 6, tid==pid? 1
```

**Explanation:**

- `shared` is a global used by everyone → it accumulates 3, then 6.
- `mine` is `_Thread_local` → each thread has its own copy starting at 0; main's copy is untouched → 0.
- `getpid()` is the same in every thread (the TGID). The other threads' TIDs differ from the PID; only the main thread has TID = PID.

**Exam relevance:** Threads share: code, data, heap, fds, signal handlers, cwd, PID. Each has its own: stack, registers/PC, TID, signal mask, errno (errno is thread-local). On Linux both processes and threads are created with `clone()` using different sharing flags.
