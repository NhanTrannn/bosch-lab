# Linux: signals — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

### 01_sigint_handler — Catching Ctrl+C with sigaction

**Question:** What happens on the 1st and 2nd Ctrl+C? On which press does the program exit, and what does the last line print?

**Expected output:**

```text
PID 9001. Press Ctrl+C 3 times to quit.
running... count = 0
running... count = 0
^C [handler: got SIGINT]
running... count = 1
^C [handler: got SIGINT]
running... count = 2
^C [handler: got SIGINT]
Clean exit after 3 Ctrl+C presses
```

**Explanation:**

- Ctrl+C makes the terminal send **SIGINT** (2) to the whole foreground process group. The handler replaces the default action (terminate).
- `sleep()` is interrupted by the signal and returns early, so the loop checks `stop` right away.
- `volatile`: forces the variable to be re-read from memory every time (not cached in a register). `sig_atomic_t`: reads/writes are atomic with respect to signals.
- `printf` is not async-signal-safe (it uses a lock and an internal buffer) → the handler uses `write`.

**Exam relevance:** The old `signal()` behaves differently between systems; `sigaction()` is the POSIX standard and lets you set `sa_mask`, `SA_RESTART`, `SA_SIGINFO`.

### 02_sigchld_reap — Reaping zombies with SIGCHLD ⚠️ nondeterministic

**Question:** Does the program print "Reaped all 5 children"? If the handler called waitpid only once (no while loop), what could go wrong?

**Expected output:**

```text
(the number of lines in the middle may vary between runs)
parent working... reaped 0 children
parent working... reaped 1 children
parent working... reaped 2 children
parent working... reaped 3 children
parent working... reaped 4 children
Reaped all 5 children, no zombies left
```

**Explanation:**

- Every time SIGCHLD arrives, `sleep` is interrupted, so the parent prints its progress again.
- `WNOHANG`: if no child has exited, return 0 immediately (do not block). `-1` = any child.
- With only one `waitpid`: when 2 children exit almost at once, the second SIGCHLD is merged (pending is just 1 bit) → one child stays a zombie and `reaped` never reaches 5 → it hangs.
- Save/restore `errno`, because the handler may interrupt code that is checking `errno`.

**Exam relevance:** Standard signals are not queued; real-time signals (`SIGRTMIN..SIGRTMAX`) are. SIGCHLD's default action is **ignore**.

### 03_kill_test — Sending signals with the kill command

**Question:** What does the first line print (sigaction for SIGKILL)? For each command kill -TERM, -USR1, -STOP, -CONT, -KILL from terminal 2: how does the program react?

**Expected output:**

```text
sigaction(SIGKILL): Invalid argument
PID = 9500. In terminal 2, try in turn:
  ...
tick 0
tick 1
[handler] SIGTERM -> still running     <- kill -TERM
tick 2
[handler] SIGUSR1                      <- kill -USR1
tick 3
                                       <- kill -STOP: the ticks stop (ps shows STAT = T)
tick 4                                 <- kill -CONT: it continues
Killed                                 <- kill -KILL: dies at once, the shell prints "Killed"
```

**Explanation:**

- `sigaction(SIGKILL, ...)` returns -1 with `errno = EINVAL` → prints `Invalid argument`.
- SIGTERM is caught, so the process does **not** die. That is why `kill <pid>` (TERM by default) sometimes fails to kill a process.
- SIGSTOP stops the process (it cannot be caught), SIGCONT lets it continue.
- SIGKILL is handled directly by the kernel; the program gets no chance to clean up.

**Exam relevance:** Learn the numbers on Linux x86: HUP 1, INT 2, QUIT 3, ABRT 6, KILL 9, USR1 10, SEGV 11, USR2 12, PIPE 13, ALRM 14, TERM 15, CHLD 17, CONT 18, STOP 19, TSTP 20.

### 04_alarm_pause — alarm, pause and the default action of SIGALRM

**Question:** What does alarm(0) return? What do pause() and errno return? Does the last line "not printed" appear, and what does the shell report?

**Expected output:**

```text
alarm(0) returns 10
alarm(1), then pause()...
  [handler] SIGALRM
pause() = -1, errno = Interrupted system call
alarm(1) again, this time WITHOUT catching SIGALRM
Alarm clock
```

**Explanation:**

- `alarm(0)` cancels the pending timer and returns the seconds that were left (10).
- After 1 second SIGALRM arrives → the handler runs → pause() returns -1 with `errno = EINTR`.
- The second time SIGALRM is not caught → its default action **terminates** the process. The shell prints "Alarm clock" and `echo $?` = 142 (128 + 14). "not printed" does not appear.

**Exam relevance:** Default actions: Term (SIGINT, SIGTERM, SIGALRM, SIGPIPE, SIGUSR1/2, SIGHUP, SIGKILL), Core (SIGSEGV, SIGABRT, SIGFPE, SIGQUIT), Ign (SIGCHLD), Stop (SIGSTOP, SIGTSTP), Cont (SIGCONT). The exit code after dying from signal n is 128 + n.

### 05_sigprocmask — Blocking signals (sigprocmask), pending signals, and why signals are not queued

**Question:** SIGUSR1 is sent 3 times while blocked → after unblocking, how many times does the handler run? And with the real-time signal SIGRTMIN?

**Expected output:**

```text
blocked: SIGUSR1 pending? 1, SIGRTMIN pending? 1
before unblocking: usr1 = 0, rt = 0
after unblocking:  usr1 = 1, rt = 3
```

**Explanation:**

- A blocked signal is not delivered; it becomes pending → the handlers have not run yet (0, 0).
- A standard signal has just one pending bit → the 3 raises merge into 1.
- Real-time signals are queued → delivered all 3 times, and they can carry data (`sigqueue`).
- Pending signals are delivered as soon as `sigprocmask(SIG_UNBLOCK)` returns.

**Exam relevance:** Distinguish **blocking** (postponed, still pending) from **ignoring** (thrown away). SIGKILL and SIGSTOP cannot be blocked, ignored or caught. In threads use `pthread_sigmask`.
