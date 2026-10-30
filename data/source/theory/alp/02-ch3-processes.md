---
tags: [linux, alp, processes, fork, exec, signals]
---

# ALP ch.3: Processes

## Theory

Follows *Advanced Linux Programming*, chapter 3 "Processes". Labs: `alp/02`, `07`, `08`; see also `linux-process/*`, `linux-signal/*`. The "Processes, virtual memory and scheduling" and "Signals and IPC" topics above give an overview.

### Looking at processes (3.1)
- Each process has a unique **PID** (type `pid_t`, `<sys/types.h>`) and a **PPID** (its parent's PID). `getpid()`, `getppid()`.
- `ps` (only the current terminal's processes by default), `ps -e -o pid,ppid,command` (all processes, chosen columns), `ps aux`, `ps -ef`. The root process is `init` (PID 1).
- `kill PID` sends **SIGTERM** (the default); `kill -KILL PID` / `kill -9 PID` sends SIGKILL. From C: `kill(pid, sig)` (`<signal.h>`).

### Creating processes (3.2)
- **`system("command")`** (`<stdlib.h>`): runs the command via `/bin/sh -c`, waits for it, returns the wait status (127 if the shell could not run the command). Convenient, but **the book advises against it**: it depends on the system shell, it is slow, and it is dangerous when the command string contains user input (a setuid program + system = a security hole).
- **`fork()`**: creates a copy of the process. Returns **the child's PID in the parent**, **0 in the child**, -1 on error. The child inherits memory (copy-on-write), open fds, the environment and signal handlers; its PID, PPID, pending signals and CPU times differ.
- **The exec family** replaces the running program with a new one (same PID). On success it **never returns**.
  - `p` (`execvp`, `execlp`): searches `PATH` for the program; without `p` you need the full path.
  - `v` (`execv`, `execvp`): argv is a NULL-terminated array.
  - `l` (`execl`, `execlp`, `execle`): arguments listed directly, ending with `NULL`.
  - `e` (`execve`, `execle`): also passes an array of environment strings.
  - The first argv element should be the program name.
- **fork + exec pattern** (listing 3.4 `spawn`): the child calls `execvp(program, arg_list)`; if exec returns (an error), it prints the error and calls `abort()`.
- **Scheduling**: Linux schedules parent and child independently; there is no guarantee which runs first. **Niceness** ranges from -20 (highest priority) to 19, default 0. `nice -n 10 prog`, `renice -n 10 -p PID`, the function `nice(incr)`. Only root can lower niceness (raise priority).

### Signals (3.3)
- A signal is an **asynchronous** message sent to a process. On receipt the process does one of: the default action (terminate, ignore, stop, core dump), ignore it, or call a **handler**.
- Common signals: `SIGINT` (Ctrl+C), `SIGTERM` (please terminate, the default of kill), `SIGKILL` (kill immediately, **cannot be caught or ignored**), `SIGSTOP` (stop, also uncatchable), `SIGHUP` (terminal lost, often used to ask for a config reload), `SIGUSR1`/`SIGUSR2` (user-defined), `SIGSEGV` (invalid memory access), `SIGBUS` (invalid memory access, e.g. past the end of an mmap'ed file), `SIGFPE` (arithmetic error, e.g. integer division by zero), `SIGABRT` (abort), `SIGCHLD` (a child ended), `SIGPIPE` (write to a pipe with no reader), `SIGALRM` (alarm).
- **`sigaction(sig, &new, &old)`** sets the behavior. `struct sigaction` has `sa_handler` (a `void handler(int)` function, or `SIG_DFL` = default, `SIG_IGN` = ignore), `sa_mask` (signals blocked while the handler runs) and `sa_flags` (`SA_RESTART`, `SA_SIGINFO`...). The book recommends sigaction over `signal()` (whose behavior differs between systems).
- A handler must **do very little** and return quickly, because it interrupts the program at an arbitrary point (even inside a library call). Do not call functions that are not async-signal-safe, such as printf or malloc, in a handler. Usually it just records that the signal arrived.
- A variable shared between a handler and the program should have type **`sig_atomic_t`** (assigned in a single instruction, cannot be interrupted halfway) and be `volatile`. Listing 3.5 counts SIGUSR1 deliveries this way.

### Process termination (3.4)
- Normal termination: return from main or call `exit(code)` (runs `atexit` functions, flushes stdio). `_exit` ends immediately without flushing.
- Abnormal termination by a signal: SIGINT, SIGTERM, SIGKILL, SIGABRT (from `abort()`), SIGSEGV...
- The **exit code** is only 8 bits; use 0–127. When killed by a signal the shell reports 128 + the signal number.
- **`wait(&status)`**: blocks until **any** child ends and returns its PID. **`waitpid(pid, &status, options)`**: waits for a specific child (`-1` = any); `WNOHANG` = do not block (returns 0 if no child has finished). `wait3`/`wait4`: also return resource usage (`struct rusage`).
- Status macros: `WIFEXITED(s)` + `WEXITSTATUS(s)` (exited normally, and its exit code), `WIFSIGNALED(s)` + `WTERMSIG(s)` (killed by a signal, and which one), `WIFSTOPPED`, `WCOREDUMP`.
- **Zombie**: a child that has ended but has not been waited for → it keeps its entry in the process table so the parent can collect the exit status; `ps` shows state `Z` / `<defunct>`. A zombie cannot be killed (it is already dead); once the parent waits, it disappears. If the parent dies first, its children (including zombies) are adopted by **init**, which reaps them.
- **Cleaning up children asynchronously** (listing 3.7): install a **SIGCHLD** handler that calls `wait` (storing the status in a `sig_atomic_t`). The parent does not block yet leaves no zombies. Several children dying close together may produce a single SIGCHLD → loop on `waitpid(-1, &s, WNOHANG)` until it returns ≤ 0.

## Quiz

### Which function returns the PID of the parent process? {#alp-ch3-001}
- [ ] `getpid()`
- [x] `getppid()`
- [ ] `getuid()`
- [ ] `getpgid()`
> getpid() is the process's own PID; getuid() is the user ID; getpgid() is the process group ID.

### Which signal does `kill 1234` (no signal given) send? {#alp-ch3-002}
- [ ] SIGKILL
- [x] SIGTERM
- [ ] SIGINT
- [ ] SIGHUP
> SIGTERM can be caught, so the program can clean up. `kill -9`/`kill -KILL` is SIGKILL.

### Why does the book advise avoiding `system()` in real programs? (select all that apply) {#alp-ch3-003}
- [x] It depends on the system's `/bin/sh`, whose behavior can vary
- [x] It is dangerous when the command string contains user data (shell injection), especially in setuid programs
- [x] It costs an extra shell process
- [ ] system() does not return an exit code
> system returns a wait status (read it with WEXITSTATUS). Use fork + exec to control exactly which program runs with which arguments.

### What does fork() return in the child process? {#alp-ch3-004}
- [ ] The child's PID
- [ ] The parent's PID
- [x] 0
- [ ] -1
> The parent gets the child's PID (> 0). -1 means fork failed (no child was created).

### How many "hello" lines does this print? {#alp-ch3-005}
```c
fork();
fork();
printf("hello\n");
```
- [ ] 2
- [ ] 3
- [x] 4
- [ ] 8
> Each fork doubles the number of processes: 1 → 2 → 4, and each prints one line.

### What does a child NOT inherit from its parent after fork? {#alp-ch3-006}
- [ ] Open file descriptors
- [ ] Environment variables
- [x] The PID
- [ ] Installed signal handlers
> The child gets a new PID; its PPID is the parent's PID. Pending signals and fcntl file locks are not inherited either.

### When is `printf("after exec\n");` printed? {#alp-ch3-007}
```c
execlp("ls", "ls", "-l", NULL);
printf("after exec\n");
```
- [ ] Always, after ls finishes
- [ ] Never
- [x] Only if execlp fails
- [ ] Before ls runs
> A successful exec replaces the whole program and never returns. So the code after exec is usually error handling (listing 3.4 calls abort).

### Which exec function searches PATH and takes its arguments as an array? {#alp-ch3-008}
- [ ] `execv`
- [x] `execvp`
- [ ] `execl`
- [ ] `execle`
> p = PATH, v = vector (array). execv needs a full path; execl/execle take a list of arguments.

### What must the argument list passed to `execl` end with? {#alp-ch3-009}
- [ ] An empty string ""
- [x] A NULL pointer
- [ ] An int 0
- [ ] Nothing
> Usually written `(char *)NULL` to have the right type on every platform. An argv array for execv must end with NULL too.

### Which exec function lets you pass a completely new environment to the program it runs? {#alp-ch3-010}
- [ ] `execvp`
- [ ] `execlp`
- [x] `execle` (and `execve`)
- [ ] `execv`
> The e suffix = environment. The others use the process's current `environ`.

### After fork, does the parent or the child run first? {#alp-ch3-011}
- [ ] Always the parent
- [ ] Always the child
- [x] Unspecified; the scheduler decides
- [ ] The parent waits until the child is done
> If you need an order, synchronize (wait, a pipe, a signal, a semaphore).

### Which niceness value gives a process the HIGHEST priority? {#alp-ch3-012}
- [x] -20
- [ ] 0
- [ ] 19
- [ ] 20
> The higher the niceness, the more the process "yields" the CPU. Normal users can only raise niceness; lowering it (raising priority) needs root.

### Which signals can NOT be caught, blocked or ignored? (select all that apply) {#alp-ch3-013}
- [x] SIGKILL
- [x] SIGSTOP
- [ ] SIGTERM
- [ ] SIGINT
> They are the system's last resort, so a process can always be stopped or killed.

### Which signal does Ctrl+C send to the foreground process in a terminal? {#alp-ch3-014}
- [ ] SIGTERM
- [x] SIGINT
- [ ] SIGQUIT
- [ ] SIGTSTP
> Ctrl+\ sends SIGQUIT (with a core dump), Ctrl+Z sends SIGTSTP (stops the process).

### Accessing past the end of a file in an mmap'ed region usually causes which signal? {#alp-ch3-015}
- [ ] SIGSEGV
- [x] SIGBUS
- [ ] SIGFPE
- [ ] SIGPIPE
> SIGSEGV is for unmapped addresses or wrong permissions; SIGBUS is for mapped addresses with no real page behind them.

### Integer division by zero on Linux x86 causes which signal? {#alp-ch3-016}
- [ ] SIGSEGV
- [x] SIGFPE
- [ ] SIGABRT
- [ ] No signal, the result is 0
> FPE (floating-point exception) is a historical name; the signal covers every arithmetic error.

### Which type does the book recommend for a variable shared between a signal handler and the main program? {#alp-ch3-017}
- [ ] `int`
- [ ] `atomic_int`
- [x] `sig_atomic_t` (with volatile)
- [ ] `pthread_mutex_t`
> sig_atomic_t is read and written in a single machine instruction, so a signal cannot interrupt it halfway. Do not use a mutex in a handler (it can deadlock).

### Why should a signal handler not call printf or malloc? {#alp-ch3-018}
- [ ] Handlers have no stack
- [x] The handler may interrupt the program right inside printf/malloc; calling them again corrupts their internal state (they are not async-signal-safe)
- [ ] printf is forbidden in every static function
- [ ] printf always returns EINTR
> Only async-signal-safe functions (write, _exit, ...) belong in a handler; better still, just set a flag and handle it in the main loop.

### In struct sigaction, what does `sa_handler = SIG_IGN` mean? {#alp-ch3-019}
- [ ] Restore the default behavior
- [x] Ignore the signal
- [ ] Block the signal until it is unblocked
- [ ] Call the library's default handler
> SIG_DFL is the default action. Blocking (with sigprocmask) keeps a signal pending and delivers it later, whereas ignoring throws it away.

### How do `exit()` and `_exit()` differ? {#alp-ch3-020}
- [ ] No difference
- [x] exit() runs the atexit functions and flushes stdio buffers before entering the kernel; _exit() ends immediately
- [ ] _exit() is only for threads
- [ ] exit() does not close file descriptors
> A child after fork (when exec fails) should use `_exit`, so it does not flush the stdio buffers copied from the parent (duplicated output).

### A child calls `exit(258)`. What does the parent read from `WEXITSTATUS(status)`? {#alp-ch3-021}
- [ ] 258
- [x] 2
- [ ] 255
- [ ] 0
> Only the low 8 bits survive: 258 mod 256 = 2.

### A child calls `abort()`. Which macros apply? {#alp-ch3-022}
- [ ] WIFEXITED(status) is true, WEXITSTATUS = 6
- [x] WIFSIGNALED(status) is true, WTERMSIG = SIGABRT (6)
- [ ] WIFSTOPPED(status) is true
- [ ] WEXITSTATUS = 134
> A process killed by a signal has no exit code. 134 = 128 + 6 is only how the shell reports it in `$?`.

### What does it mean when `waitpid(pid, &status, WNOHANG)` returns 0? {#alp-ch3-023}
- [ ] The child exited with code 0
- [x] The child is still running; its state has not changed
- [ ] An error, check errno
- [ ] There are no children
> With no children at all it returns -1 with errno = ECHILD.

### What is a zombie process? {#alp-ch3-024}
- [ ] A process whose parent has died
- [x] A process that has ended but whose parent has not yet called wait to collect its exit status
- [ ] A hung, unresponsive process
- [ ] A process left running in the background after closing the terminal
> A process whose parent died is an orphan (adopted by init). A zombie uses no CPU or memory but takes an entry/PID in the process table.

### How do you get rid of a zombie? {#alp-ch3-025}
- [ ] `kill -9` the zombie
- [x] Have the parent call wait/waitpid, or end the parent (init adopts and reaps the zombie)
- [ ] Restart the terminal
- [ ] `nice -n 19` the zombie
> A zombie is already dead, so kill has no effect.

### How can a parent avoid zombies without blocking in wait? {#alp-ch3-026}
- [ ] Call sleep in the parent
- [x] Install a SIGCHLD handler that calls wait/waitpid
- [ ] Call exit in the parent
- [ ] Block SIGCHLD
> Listing 3.7. Since several SIGCHLDs can merge into one, loop `while (waitpid(-1, &s, WNOHANG) > 0);`. Setting SIGCHLD to SIG_IGN also makes the kernel reap them (but then you cannot get the exit status).

### What does `wait(&status)` do in a process with 3 running children? {#alp-ch3-027}
- [ ] Waits for all 3 children to finish
- [x] Blocks until any one child finishes and returns that child's PID
- [ ] Returns -1 immediately
- [ ] Waits for the first child that was created
> To wait for all of them, call wait 3 times (or loop until it returns -1/ECHILD).
