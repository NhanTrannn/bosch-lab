# Linux: processes — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

### 01_fork_count — fork() three times ⚠️ nondeterministic

**Question:** How many "PID = ..." lines are printed? How many different PPIDs are there?

**Expected output:**

```text
8 lines in an unpredictable order, e.g.:
PID = 5001, PPID = 4990     <- the original process (PPID = shell/make)
PID = 5002, PPID = 5001
PID = 5003, PPID = 5001
PID = 5005, PPID = 5001
PID = 5004, PPID = 5002
PID = 5006, PPID = 5002
PID = 5007, PPID = 5003
PID = 5008, PPID = 5004
```

**Explanation:**

- 1 → 2 → 4 → 8 processes. The tree: the root has 3 children, the first child has 2 children, ...
- There are 5 different PPIDs (the root's parent, the root, and the 3 other processes that have children: every process created by the 1st or 2nd fork has children).
- The print order depends on the scheduler → **nondeterministic**.

**Exam relevance:** Formula: n consecutive `fork()` calls → 2^n processes, and a `printf` afterwards prints 2^n times; the number of **child processes created** = 2^n − 1.

### 02_fork_if — fork() inside if and inside logical expressions ⚠️ nondeterministic

**Question:** How many lines does part [1] print? How many does part [2] print with fork() && fork() || fork()?

**Expected output:**

```text
[1] appears 3 times
-----
[2] appears 5 times
(PIDs and line order vary)
```

**Explanation:**

- Part 1: the root forks C1 (the parent gets > 0, so it does not enter the if). C1 gets 0 → forks again, creating C2. 3 processes in total.
- Part 2, calling the three calls A, B, C:
  - Root: A > 0 → runs B (creates P2). In the root B > 0 → `&&` is true → C is skipped.
  - P2: B = 0 → `&&` is false → runs C (creates P3).
  - P1 (A's child): A = 0 → skips B, runs C (creates P4).
  - Total: root, P1, P2, P3, P4 = **5**.

**Exam relevance:** "How many processes are created" questions are very common. Draw the tree and write the return value (0 or > 0) on each branch.

### 03_fork_buffer — stdio buffers and fork

**Question:** What appears on screen? How many times does the letter A appear?

**Expected output:**

```text
AB
AB
```

**Explanation:**

- `"A"` has no `\n`, so it is still in `stdout`'s buffer. `fork()` copies all of user-space memory, **buffer included** → both parent and child have an "A" waiting to be printed.
- Fix: `fflush(stdout)` before `fork()` → "A" is printed once: `AB` then `B`.
- With `printf("A\n")`: on a terminal it prints `A, B, B`; but redirected to a file (fully buffered) it becomes `A B A B`.
- `write(1, ...)` is a system call with no user-space buffer → nothing is duplicated.

**Exam relevance:** "Why is the output duplicated when redirected to a file?" → the stdio buffer is duplicated by `fork`. Remember: `exit()` flushes buffers, `_exit()` does not.

### 04_fork_exec_wait — fork + exec + waitpid

**Question:** What exit code (WEXITSTATUS) does the parent print? Does the perror("execlp") line run?

**Expected output:**

```text
Child (pid 6120) will run ls
ls: cannot access '/does_not_exist': No such file or directory
Parent: child 6120 exited with code 2
```

**Explanation:**

- `execlp` succeeds (it finds `ls` in PATH) → the child becomes `ls`, and the lines after `execlp` **never run**.
- `ls` exits with code 2 on a serious error (an argument it cannot access). A valid directory → 0.
- With `lsx`: exec fails → `perror` prints `execlp: No such file or directory` → exit code 127 (the shell's convention for "command not found").
- Only the low 8 bits of the exit code are kept: `exit(256)` → the parent sees 0.

**Exam relevance:** The suffixes: **l** = argument list, **v** = `argv` array, **p** = search PATH, **e** = pass an environment. They all end up calling the `execve` system call.

### 05_zombie — Zombie processes

**Question:** During those 30 seconds, in a second terminal, what does the STAT column show for the child? How is its command name displayed?

**Expected output:**

```text
Terminal 1:
Parent 7001: sleeping 30s WITHOUT calling wait(). Child = 7002
Terminal 2: ps -o pid,ppid,stat,cmd --ppid 7001
Child 7002: exiting right away
(after 30 seconds) Parent exits

Terminal 2:
    PID    PPID STAT CMD
   7002    7001 Z+   [05_zombie] <defunct>
```

**Explanation:**

- STAT `Z` = zombie, and the command name shows `<defunct>`. A zombie uses no CPU/RAM but holds a **PID** — too many of them exhaust the PIDs.
- `kill -9` on a zombie **has no effect**: it is already dead.
- When the parent exits, the zombie becomes an orphan → it is adopted by `init/systemd` (or a subreaper), which calls `wait` to clean it up.
- How to avoid it: call `wait/waitpid`, handle `SIGCHLD`, or `signal(SIGCHLD, SIG_IGN)`.

**Exam relevance:** Process states in `ps`: R (running), S (interruptible sleep), D (uninterruptible sleep, usually waiting for I/O), T (stopped), Z (zombie).

### 06_orphan — Orphan processes ⚠️ nondeterministic

**Question:** How do the PPIDs the child prints on the "before" and "after" lines differ? Where does the shell prompt appear?

**Expected output:**

```text
Parent 8100: exiting after 1s
Child 8101: before, PPID = 8100
user@ubuntu:~/bosch-lab$ Child 8101: after,  PPID = 1      <- or the PID of "systemd --user"
```

**Explanation:**

- At first the PPID is the parent's PID. The parent exits after 1 second → the child becomes an orphan and is adopted.
- On an Ubuntu desktop the PPID is usually the PID of `systemd --user` (a **subreaper**), not necessarily 1. In Docker/WSL it may be 1.
- The shell only waits for the parent, so the prompt appears **before** the child's "after" line. (In the web terminal the run ends when the parent exits, so the "after" line may not be shown.)

**Exam relevance:** Zombie: the child is dead, the parent is alive but does not wait. Orphan: the parent is dead, the child is alive. Orphans are harmless; accumulating zombies are not.

### 07_fork_cow — Variables after fork(): copy-on-write ⚠️ nondeterministic

**Question:** What do g, local and heap print in the parent after the child incremented all three? Are the &g addresses the same in parent and child?

**Expected output:**

```text
child:  g=11 local=21 heap=31 | &g=0x5870e837b010 heap=0x587110e802a0
parent: g=10 local=20 heap=30 | &g=0x5870e837b010 heap=0x587110e802a0
(the addresses change on every run, but they are identical in parent and child)
```

**Explanation:**

- The child modifies its own copy; the parent still sees the old values 10/20/30.
- The printed addresses are **identical** because they are virtual addresses — the child has its own page table pointing to its own physical pages. The concrete numbers change on every run (ASLR), but parent and child always match.
- Copy-on-write: after fork, the two processes share physical pages read-only; only when one of them writes does the kernel copy that page. That is why fork is fast.

**Exam relevance:** A child inherits: a copy of memory, open fds, signal handlers, cwd, environment variables. A child does NOT inherit: the PID, pending signals, file locks (fcntl), timers (alarm), other threads.

### 08_fork_fd_offset — File descriptors after fork(): a shared offset

**Question:** What does file 1 contain (parent and child share an fd opened BEFORE fork)? What does file 2 contain (each side opens it on its own AFTER fork)?

**Expected output:**

```text
/tmp/bosch_offset1.txt (13 bytes):
child
parent
/tmp/bosch_offset2.txt (9 bytes):
parent
X
```

**Explanation:**

- File 1: the fd is duplicated, but both copies point to the same open file description → a shared offset. The child writes 6 bytes, the offset becomes 6, and the parent continues writing from 6 → "child\nparent\n".
- File 2: each side `open`s it separately → separate offsets starting at 0. The parent's "parent\n" (7 bytes) overwrites the first 7 bytes of "XXXXXXXX\n", leaving "parent\nX\n".

**Exam relevance:** Three levels: the fd table (per process) → the open file table (offset, flags; shared after fork/dup) → the inode. `dup2(fd, 1)` is how the shell implements redirection.

### 09_exec_buffer — exec() replaces the program: the PID stays, the stdio buffer is lost ⚠️ nondeterministic

**Question:** Does the "no newline yet..." text appear? Does the PID change after exec? Is the "never printed" line printed?

**Expected output:**

```text
PID before exec = 10144
PID after exec  = 10144
(the PID changes on every run, but the two lines always show the same number)
```

**Explanation:**

- The first line ends in `\n` and stdout is a terminal (line-buffered) → it was flushed before exec.
- "no newline yet..." is still in the stdio buffer (user-space memory) → exec wipes all the old memory → it is lost.
- `$$` in `sh -c` is sh's PID = the old PID, because exec does not create a new process.
- A successful exec never returns → the last two lines do not run.

**Exam relevance:** fork creates a new process (a copy); exec replaces the insides of the current process. A shell runs a command with fork + exec in the child + wait in the parent. `vfork`/`posix_spawn` optimize the exec-right-after-fork case.
