# Advanced Linux Programming, chapters 1–5 — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

Labs following the book *Advanced Linux Programming* (Mitchell, Oldham, Samuel, 2001), chapters 1–5. Function names and examples follow the book's listings.

### 01_getopt_long — getopt_long: short/long options, optarg, optind, "--" and argv permutation

**Question:** For each sample argv, which options does getopt_long return, what is optarg, and where does optind stop? How is "-vo x.txt" understood? After "--", is "-h" still an option? What happens with "-x" (not declared) and "-o" without its argument?

**Expected output:**

```text
Sample 1: -v -o out.txt in1 in2
  -v/--verbose
  -o/--output, optarg = out.txt
  optind = 4, remaining arguments: in1 in2
Sample 2: --output=log.txt --verbose file
  -o/--output, optarg = log.txt
  -v/--verbose
  optind = 3, remaining arguments: file
Sample 3: -vo x.txt -- -h
  -v/--verbose
  -o/--output, optarg = x.txt
  optind = 4, remaining arguments: -h
Sample 4: in1 -v in2 --output o.txt
  -v/--verbose
  -o/--output, optarg = o.txt
  optind = 4, remaining arguments: in1 in2
  argv after permutation: -v --output o.txt in1 in2
Sample 5: -x -o
prog: invalid option -- 'x'
  '?' (optopt = 'x')
prog: option requires an argument -- 'o'
  '?' (optopt = 'o')
  optind = 3, remaining arguments:
```

**Explanation:**

- `"ho:v"`: `o` has a `:` so it needs an argument, either the next word (`-o out.txt`) or attached (`-oout.txt`). Long options: `--output=log.txt` or `--output log.txt`.
- `-vo x.txt`: combined short options; `v` takes no argument, `o` takes the next word.
- `--` ends the options: everything after it (even `-h`) is a plain argument.
- GNU getopt **permutes** argv: the plain arguments (`in1`, `in2`) are moved to the end, so after the loop you only need to walk from `optind` to `argc`.
- An undeclared option or a missing argument → `'?'` is returned, the offending character is in `optopt`, and an error message goes to **stderr** (turn it off with `opterr = 0`). Listing 2.2 then calls `print_usage(stderr, 1)`.

**Exam relevance:** GNU conventions: short option `-h`, long option `--help`; every program should support `--help` and `--version`. The `struct option` array must end with `{NULL, 0, NULL, 0}`. The `has_arg` field is 0 (none), 1 (required) or 2 (optional).

### 02_env_exit_code — Environment variables and exit codes, including death by signal

**Question:** Does setenv(..., 0) overwrite? Do the child process and a program it execs see the variable just set? What WEXITSTATUS does system("exit 300") give? When the shell is killed by SIGTERM, what is the wait status, and what $? does the parent shell see?

**Expected output:**

```text
environ has > 0 variables, PATH is set
SERVER_NAME = (NULL) -> using the default server.my-company.com
after setenv: SERVER_NAME = backup.example.net
child process sees: backup.example.net
program after exec sees: backup.example.net
after unsetenv: NULL
system("exit 3"): WIFEXITED, WEXITSTATUS = 3
system("exit 300"): WIFEXITED, WEXITSTATUS = 44
system("kill -TERM $$"): WIFSIGNALED, WTERMSIG = 15 (Terminated)
Terminated
  the shell sees $? = 143
system("sh -c 'kill -TERM $$'; echo "  the shell sees \$? = $?"; exit 0"): WIFEXITED, WEXITSTATUS = 0
system("no_such_command 2>/dev/null"): WIFEXITED, WEXITSTATUS = 127
```

**Explanation:**

- `setenv(name, value, 0)`: does not overwrite an existing variable.
- The environment is copied to the child at fork and kept across exec (exec without the `e` suffix).
- `getenv` returns NULL when the variable is missing → the program uses a default (listing 2.4).
- Only the low 8 bits of the exit code survive: 300 mod 256 = 44. That is why the book recommends 0–127 only.
- A shell killed by SIGTERM: the wait status says "killed by signal 15", with no exit code. The parent shell then reports `$?` = 128 + 15 = 143 and prints "Terminated".
- Command not found: the shell returns 127 (126 if the file exists but cannot be executed).

**Exam relevance:** `printenv`, `export VAR=value`, `echo $?`. The `environ` variable (`extern char **environ;`) is a NULL-terminated array of `"NAME=value"` strings; do not modify it directly — use `setenv`/`unsetenv`. A child process can never change its parent's environment.

### 03_stdio_buffer — stdio buffering: full, line, unbuffered; exit vs _exit

**Question:** With stdout set to full buffering, in what order do the [stdout] and [stderr] parts appear? In the second part, in what order do A/B/C/D/E appear? Does the last line appear?

**Expected output:**

```text
[stderr 2] [stderr 4]
[stdout 1] [stdout 3]
C(none) B(line) D(line)
E(none)
A(full)
```

**Explanation:**

- stdout is fully buffered: `[stdout 1]` and `[stdout 3]` stay in the buffer until `fflush`. stderr is unbuffered, so it comes out first.
- Second part: `C` (unbuffered) comes out immediately; `B` stays in the line-buffered stream until the `'\n'` of `D`; `E` comes out immediately; `A` (fully buffered) only comes out at `fclose`.
- The last line is in stdout's buffer; `_exit` does not flush stdio, so it is lost. `exit()` (or returning from main) would flush it.

**Exam relevance:** stdout to a terminal is line-buffered, to a file/pipe fully buffered (so `./app | cat` can change the order relative to stderr). The book's example: `while (1) { printf("."); sleep(1); }` does not print one dot per second; switching to stderr does. Merging both streams: `prog > out.txt 2>&1` (2>&1 **after** a file redirection) and `prog 2>&1 | filter` (**before** a pipe).

### 04_tempfile — Safe temporary files: mkstemp + immediate unlink, tmpfile

**Question:** What permissions does mkstemp give the file? After unlink, does access() still see the file, and can you still read through the fd? What is st_nlink before and after unlink?

**Expected output:**

```text
file name: /tmp/alp_temp.<6 chars>, still XXXXXX ? no, length = 20
mode = 600, st_nlink = 1
after unlink: access() = -1 (No such file or directory), st_nlink = 0
read back through the fd: length = 15, data = "temporary data"
tmpfile: read back 42 tmpfile, fileno(f) > 2 ? 1
```

**Explanation:**

- The name template must end with exactly 6 `X`s; mkstemp modifies the array in place (so it must be a `char[]`, not a string literal).
- Mode 0600: only the owner can read/write.
- `unlink` removes the name (the link count drops to 0) but the inode lives on because the fd is still open → it can still be read and written. Only when the fd is closed or the process dies is the file really deleted, so no junk is left in /tmp.
- Listing 2.5 writes the length first, then the data, `lseek`s back to the start and reads it back.
- `tmpfile()` returns a `FILE*` that is already unlinked.

**Exam relevance:** Three requirements for temporary files: no name collisions between instances, permissions that stop others from tampering, unpredictable names (no race between checking and opening). Use mkstemp (an fd, the name can be handed to another program) or tmpfile (a FILE*, internal use only). Do not use mktemp, tmpnam, tempnam.

### 05_errno_assert — errno, strerror, perror and assert ⚠️ dies on purpose with SIGABRT

**Question:** What is errno after each failing call? After a SUCCESSFUL call, does errno go back to 0? With NDEBUG, how many times does assert(do_something() == 0) call the function? How does the last line end the program?

**Expected output:**

```text
open("/does/not/exist")      -> -1, errno =  2 ENOENT  (No such file or directory)
getpid() (succeeds)          ->  1, errno =  2 ENOENT  (No such file or directory)
open("/tmp", O_WRONLY)       -> -1, errno = 21 EISDIR  (Is a directory)
close(12345)                 -> -1, errno =  9 EBADF   (Bad file descriptor)
mkdir("/tmp")                -> -1, errno = 17 EEXIST  (File exists)
lseek(0, 0, 12345)           -> -1, errno = 22 EINVAL  (Invalid argument)
open("/etc/shadow")          -> -1, errno = 13 EACCES  (Permission denied)
perror prints to stderr: No such file or directory
NDEBUG: calls = 0
without NDEBUG: calls = 3
05_errno_assert: data/source/labs/alp/05_errno_assert.c:64: main: Assertion `pointer != NULL' failed.
```

**Explanation:**

- A failing system call returns -1 and sets errno. A successful call does **not** reset errno → after getpid errno is still ENOENT from before.
- Save errno immediately (`int saved = errno;`), because printf or any other call may change it.
- `perror("msg")` prints `msg: <strerror(errno)>` to stderr.
- With NDEBUG the preprocessor removes the asserts → `do_something()` is never called. That is why assert expressions must have no side effects (the book: store the result in a variable first, then `assert(status == 0)`).
- A false assert prints the program name, file, line, function and expression, then calls `abort()` → SIGABRT. The shell reports `Aborted (core dumped)` and exit code 134 = 128 + 6.

**Exam relevance:** Common error codes: EPERM, ENOENT, EINTR (interrupted by a signal → retry), EBADF, EACCES, EEXIST, ENOTDIR, EISDIR, EINVAL, ENOMEM, EAGAIN. Use assert for **internal** checks (NULL pointers, parameter conditions), never to validate user input.

### 06_dlopen — Loading a shared library at run time: dlopen, dlsym, dlerror, dlclose

**Question:** Can cos be called through the pointer returned by dlsym? What do you get from dlsym on a missing name, or dlopen on a missing library? What does dlerror() return when called twice in a row?

**Expected output:**

```text
cos(0) = 1.0, sqrt(2) = 1.4142
dlsym of a missing name: (nil)
dlerror() 1st call: /lib/x86_64-linux-gnu/libm.so.6: undefined symbol: no_such_function
dlerror() 2nd call: (NULL)
dlopen of a missing library: (nil), dlerror: libnosuchlib.so: cannot open shared object file: No such file or directory
dlclose = 0
```

**Explanation:**

- dlsym returns a `void*`; cast it to the right function pointer type and call it.
- Errors are reported as NULL; the details come from `dlerror()`. Once read, the error is cleared, so the second call returns NULL.
- dlopen searches LD_LIBRARY_PATH, the ldconfig cache, /lib and /usr/lib.

**Exam relevance:** Used for plugins. `RTLD_LAZY` (resolve each symbol on first call) vs `RTLD_NOW` (resolve everything up front). Functions in a C++ library must be `extern "C"`. Link with `-ldl` (merged into libc since glibc 2.34). dlopen'ing the same library several times only increments a reference count.

### 07_fork_exec — system, fork + exec, spawn, nice

**Question:** Is the line after exec printed? When execvp runs a command that does not exist, what does the child do and what status does the parent see? Is HOME set in the program run by execle? Does nice(10) in the child change the parent's priority?

**Expected output:**

```text
1) system:
  [sh] $0 = sh
  system returned 0
2) spawn("echo", ...) with fork + execvp:
  [echo] hello from the new program
  [parent] child exited normally, exit code 0
3) spawn a command that does not exist:
  [child] execvp error: No such file or directory
  [parent] child killed by signal 6 (Aborted)
4) execl (full path) and the line after exec:
  [echo] run with execl
  [parent] child exited normally, exit code 0
5) execle with a custom environment:
  [sh] GREETING=hello, HOME=(not set)
  [parent] child exited normally, exit code 0
6) nice:
  [child] nice: 0 -> 10
  [parent] nice is still 0
```

**Explanation:**

- `system()` runs the command through `/bin/sh -c` → `$0` is `sh`.
- A successful exec never returns; the code after it only runs when exec fails. In listing 3.4 the child calls `abort()` after a failed exec → the parent sees SIGABRT.
- execle uses exactly the environment array it is given: only GREETING, no HOME.
- nice is a per-process attribute; the child raising its niceness (lowering its priority) does not affect the parent. Niceness ranges from -20 (highest priority) to 19; only root can lower it.

**Exam relevance:** The exec family: `execl`, `execlp`, `execle`, `execv`, `execvp`, `execve` (the real system call). `p`: search PATH; `v`: argv as an array; `l`: arguments listed and terminated by `(char *)NULL`; `e`: pass an environment. The book recommends fork + exec over system because system depends on the shell, has security holes and is slow.

### 08_wait_status — wait/waitpid, the status macros, WNOHANG, zombies

**Question:** What WEXITSTATUS does exit(300) give? What do WIFSIGNALED/WTERMSIG report for abort() and kill -9? What does waitpid(..., WNOHANG) return while the child is still running? What state does /proc show for a child that exited but was not waited for? What about wait() when there are no children?

**Expected output:**

```text
child exit(300):       WIFEXITED   WEXITSTATUS = 44
child exit(-1):        WIFEXITED   WEXITSTATUS = 255
child abort():         WIFSIGNALED WTERMSIG = 6 (Aborted)
child kill -9:         WIFSIGNALED WTERMSIG = 9 (Killed)
WNOHANG while the child runs: waitpid = 0
child exited, not waited for: state in /proc = Z
WNOHANG 2nd call: returned the child's PID ? 1
child exit(7):         WIFEXITED   WEXITSTATUS = 7
after wait: /proc/<pid> = - (gone)
wait() with no children left: -1, errno = ECHILD
```

**Explanation:**

- Only the low 8 bits survive: 300 → 44, -1 → 255.
- A process killed by a signal has no exit code; use `WTERMSIG` to find out which signal.
- `WNOHANG`: returns 0 if no child has finished yet, without blocking (for periodic checks).
- A child that exited without being waited for → a **zombie** (`Z` in ps//proc): it only keeps its PID and exit status. After wait the entry disappears completely.
- No children left → wait returns -1 with errno = ECHILD.

**Exam relevance:** `wait` blocks until any child ends; `waitpid(pid, &status, options)` waits for a specific child (pid = -1 means any); `wait3`/`wait4` also return resource usage. If the parent dies first, the zombie child is adopted and reaped by init. Automatic reaping: a SIGCHLD handler calling wait (listing 3.7), or setting SIGCHLD to SIG_IGN.

### 09_thread_return — pthreads: passing data, return values, pthread_exit, detached threads ⚠️ nondeterministic

**Question:** What is the 1000th prime computed by the thread? What does pthread_equal(self, main) give inside the child thread? What happens when you pthread_join a detached thread? Is it safe for two character-printing threads to use structs on main's stack?

**Expected output:**

```text
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxoooooooooooooooooooo     <- 30 x and 20 o; the interleaving may differ
(30 x and 20 o; the interleaving is not fixed)
prime number 1000 = 7919
in the child thread: pthread_equal(self, main) = 0
pthread_exit((void*)7) -> join got 7
pthread_join on a detached thread: 22 (EINVAL)
```

**Explanation:**

- Listings 4.2/4.3: each thread gets its own struct through `void*`. It is safe because main joins both threads before the structs (on main's stack) go away. If main returned first, the threads would read destroyed memory (the bug the book shows in listing 4.2).
- Listing 4.4: the return value is cast to `void*` and recovered through the second argument of `pthread_join`.
- `pthread_exit(v)` is equivalent to `return v` from the thread function.
- Compare thread IDs with `pthread_equal`, not `==` (pthread_t may be a struct).
- A detached thread cannot be joined: glibc returns EINVAL (POSIX calls it undefined behavior).

**Exam relevance:** Compile with `-pthread` (the book uses `-lpthread`). pthread functions return an **error code** instead of setting errno. A joinable thread nobody joins stays a "zombie thread" until it is joined; threads whose result you do not need should be detached (`pthread_detach` or the `PTHREAD_CREATE_DETACHED` attribute).

### 10_thread_cancel — Thread cancellation: deferred, uncancelable sections, cleanup handlers

**Question:** Thread A is cancelled while sleeping: does its cleanup handler run, and what does join return? Thread B disables cancellation during a "money transfer": where is it cancelled, and is "transfer done" printed? What does pthread_cleanup_pop(1) do when the thread is not cancelled?

**Expected output:**

```text
Thread A:
  [cleanup] free buffer
  join A: PTHREAD_CANCELED
Thread B:
  [B] transfer started (cancellation disabled)
  [main] pthread_cancel(B) = 0
  [B] transfer done
  join B: PTHREAD_CANCELED
Thread C:
  [C] finished normally, pop(1) still runs the handler:
  [cleanup] free buffer
  join C: 42
```

**Explanation:**

- A is deferred cancelable (the default): it is cancelled at `usleep` (a cancellation point) → the cleanup handler runs and frees the buffer → join gets `PTHREAD_CANCELED`.
- B disables cancellation: the request is **held**; B finishes its work, re-enables cancellation and is cancelled at `pthread_testcancel()` → "never reached" is not printed. That is the idea of listing 4.6 (a transfer between two accounts must not stop halfway).
- `pthread_cleanup_pop(1)` removes the handler and runs it; `pop(0)` only removes it. push/pop must be paired in the same block (they are macros that open/close `{ }`).

**Exam relevance:** Three states: asynchronously cancelable (cancelled at any time, dangerous), synchronously/deferred cancelable (the default, cancelled at cancellation points), uncancelable. Functions: `pthread_cancel`, `pthread_setcanceltype`, `pthread_setcancelstate`, `pthread_testcancel`. In C++ the book recommends throwing an exception to leave a thread, so destructors of local objects run.

### 11_tsd_mutex — Thread-specific data and kinds of mutex, semaphores

**Question:** Whose value does each thread read with pthread_getspecific? When does the key's destructor run? What does a second lock return for an error-checking mutex, and for a recursive one? Another thread unlocking an error-checking mutex? trylock on a mutex someone holds? sem_trywait when the value is 0?

**Expected output:**

```text
Thread-specific data:
  thread1.log -> writing "Thread starting."
  [destructor] closing "thread1.log"
  thread2.log -> writing "Thread starting."
  [destructor] closing "thread2.log"
  thread3.log -> writing "Thread starting."
  [destructor] closing "thread3.log"
  main: pthread_getspecific = NULL
Mutex:
  errorcheck 1st lock: 0
  errorcheck 2nd lock: EDEADLK
  another thread unlocks errorcheck: EPERM
  errorcheck unlock (by the owner): 0
  recursive lock, lock: 0 0; unlock x3: 0 0 EPERM
  fast: trylock in the thread holding it: EBUSY
  another thread trylocks the held mutex: EBUSY
Semaphore:
  initialized to 2, sem_getvalue = 2
  after 2 sem_waits: sem_trywait = -1, errno = EAGAIN
  after sem_post: 1
```

**Explanation:**

- Listing 4.7: one key, `thread_log_key`, and each thread attaches its own log file name. `write_to_thread_log` takes no parameter yet writes to the calling thread's file. main never set anything, so it gets NULL.
- The key's destructor runs automatically when a thread ends (for non-NULL values), like listing 4.7 closing the log file.
- Error-checking: relocking in the same thread → EDEADLK instead of hanging; unlocking from a non-owner → EPERM.
- Recursive: locked 2 times means unlocked 2 times; a 3rd unlock → EPERM.
- trylock never blocks: a held mutex (even held by yourself) → EBUSY.
- Counting semaphore: two waits bring it to 0; `sem_trywait` does not block but returns -1 with errno = EAGAIN (semaphores use errno, unlike mutexes).

**Exam relevance:** The book's three kinds of mutex: fast (the default; relocking deadlocks), recursive, error-checking. `PTHREAD_MUTEX_INITIALIZER` for static initialization. Thread semaphores: `sem_init(&s, 0, value)` (0 = shared only between the process's threads), `sem_wait`, `sem_post`, `sem_trywait`, `sem_getvalue`, `sem_destroy`. The book's job queue uses a mutex + a semaphore counting the jobs.

### 12_sysv_shm_sem — System V shared memory and semaphores

**Question:** What is shm_nattch before fork, while the child is alive, and after the child exits? Can the parent read the string the child wrote into shared memory? semop -1 with IPC_NOWAIT when the value is 0? The child adds to the semaphore with SEM_UNDO and exits: what value is left?

**Expected output:**

```text
segment: size = 4096 (= getpagesize()), nattch = 1
semop -1 IPC_NOWAIT at 0: -1, errno = EAGAIN
  [child] wrote it, nattch = 2
  [parent] read: "Hello, world (from the child)"
after the child exited: nattch = 1
  [child] +3 with SEM_UNDO, value = 3
child exited, the kernel undid it: value = 0
after the parent's shmdt: nattch = 0
after IPC_RMID: shmctl IPC_STAT -> EINVAL/EIDRM (removed)
```

**Explanation:**

- A segment's size should be a multiple of the page size (`getpagesize()`, usually 4096).
- fork makes the child inherit the attached region → nattch = 2; an exiting process detaches automatically.
- The child writes and the parent reads it immediately: the same physical memory, no copying. The semaphore makes sure the parent only reads after the child has written.
- `IPC_NOWAIT`: semop does not block but returns EAGAIN.
- `SEM_UNDO`: the kernel remembers the change and undoes it when the process exits, so the semaphore does not get stuck if a process dies halfway.
- The segment only disappears after `IPC_RMID` (and once nattch reaches 0).

**Exam relevance:** Commands: `ipcs -m` (shm), `ipcs -s` (semaphores), `ipcrm`. Shared memory is the **fastest** IPC because reads/writes do not go through the kernel, but you must synchronize yourself. `shmget(key, size, IPC_CREAT | IPC_EXCL | perms)`, `shmat(id, NULL, 0)`, `shmdt(addr)`, `shmctl(id, IPC_RMID, NULL)`. semop takes an array of `struct sembuf {sem_num, sem_op, sem_flg}`; semctl needs a hand-declared `union semun`.

### 13_mmap_shared_private — mmap: MAP_SHARED, MAP_PRIVATE, msync, sharing through fork

**Question:** Does writing to the MAP_PRIVATE region change the file? What does the MAP_SHARED region show? After writing through MAP_SHARED, what does pread of the file show? The child increments a counter 1000 times in a MAP_SHARED|MAP_ANONYMOUS region: what does the parent see, and with MAP_PRIVATE?

**Expected output:**

```text
at first: shared = "original", private = "original"
write via MAP_PRIVATE: private = "via PRIVATE", shared = "original", file = "original"
msync = 0
write via MAP_SHARED: shared = "via SHARED", file = "via SHARED", private = "via PRIVATE"
after the child incremented 1000 times: MAP_SHARED = 1000, MAP_PRIVATE = 0
```

**Explanation:**

- MAP_PRIVATE is copy-on-write: the first write creates a private copy of the page; the file and other mappings do not see it.
- MAP_SHARED writes straight into the file's page cache → `pread` sees it immediately. `msync(MS_SYNC)` waits until it is on disk.
- The private page has already been copied, so it no longer sees changes to the file.
- An anonymous shared mapping is inherited across fork and shared → the parent sees 1000. A private mapping is copy-on-write after fork → the parent still sees 0.

**Exam relevance:** `mmap(addr, length, prot, flags, fd, offset)`; prot: `PROT_READ`/`PROT_WRITE`/`PROT_EXEC`; flags: `MAP_SHARED`/`MAP_PRIVATE` (pick one), `MAP_FIXED`, `MAP_ANONYMOUS`. Errors return `MAP_FAILED`, not NULL. The file must be long enough before mapping (the book uses lseek + writing one byte at the end). Accessing past the end of the file → SIGBUS. msync flags: `MS_ASYNC`, `MS_SYNC`, `MS_INVALIDATE`.

### 14_popen_fdopen — Pipes with FILE*: fdopen, dup2 + exec sort, popen/pclose

**Question:** When do the lines sent to sort appear, and in what order? What status does pclose return for popen("...; exit 3", "r")? When does the child's fgets loop on the pipe stop?

**Expected output:**

```text
1) pipe + fdopen:
  [child] got: Hello, world #1
  [child] got: Hello, world #2
  [child] got: Hello, world #3
  [child] fgets returned NULL: EOF because the parent closed the write end
2) dup2 + execlp("sort"):
  [parent] wrote all 5 lines; sort prints nothing yet because it has not seen EOF
  Hello, world.
  My dog has fleas.
  One fish, two fish.
  This is a test.
  This program is great.
3) popen("sort", "w"):
  apple
  banana
  cherry
  pclose = 0 (WEXITSTATUS 0)
4) popen(..., "r"):
  read "42", pclose -> WEXITSTATUS 3
```

**Explanation:**

- fgets returns NULL once every write end is closed (the child closed its own `fds[1]`, the parent fclose'd its copy).
- sort must read all of its input (until EOF) before it can print → it only prints after the parent's `close(fds[1])`.
- popen with "w": we write to the command's stdin; `pclose` closes the pipe (sending EOF), waits for the child and returns its wait status.
- popen with "r": we read the command's stdout; the command's exit code comes from `WEXITSTATUS(pclose(...))`.

**Exam relevance:** A pipe is one-way, FIFO-ordered, and only usable between related processes (named FIFOs from `mkfifo` lift that restriction). Writing to a pipe with no reader → SIGPIPE/EPIPE. Reading an empty pipe while a write end is open → blocks. popen carries the same security risks as system because it goes through the shell.

### 15_sockets — socketpair, an Internet socket over 127.0.0.1, byte order

**Question:** Is a socketpair one-way or two-way? What is htons(80) on an x86 machine (in hex)? Which port does a server get when it binds port 0? When does read return 0?

**Expected output:**

```text
htons(80) = 0x5000, htonl(0x7f000001) = 0x0100007f, ntohs(htons(8080)) = 8080
  [child] got "ping", replying pong
  [parent] got "pong" on the same socket (two-way)
  [parent] read after the child closed its socket = 0 (EOF)
server listening on 127.0.0.1, port > 1023 ? 1
  [server] accept returned a new socket (different from the listening one ? 1), got: GET /
  [client] got: HTTP/1.0 200 OK
```

**Explanation:**

- x86 is little-endian, network byte order is big-endian → htons swaps the two bytes: 80 = 0x0050 → 0x5000.
- A socketpair is two-way: the same fd both writes and reads (unlike a pipe).
- When the other side closes the connection → read returns 0 (EOF).
- Port 0 → the kernel assigns a free ephemeral port; read it back with `getsockname`.
- `accept` returns a **new** socket for this connection; the original socket keeps listening for other clients.

**Exam relevance:** The three socket parameters: namespace (`PF_LOCAL`/`PF_UNIX`, `PF_INET`), style (`SOCK_STREAM`: connected, ordered, reliable; `SOCK_DGRAM`: datagrams that may be lost/reordered), protocol (0 = the default). Local sockets use a file path (`sockaddr_un.sun_path`) and must be `unlink`ed afterwards. Ports below 1024 need root. The book uses `gethostbyname` to resolve names (today use `getaddrinfo`).
