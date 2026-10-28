# Signals and IPC

## Theory

### Signals
- An asynchronous notification sent to a process. Sent with `kill(pid, sig)`, `raise(sig)`, the `kill -SIG pid` command, keys (Ctrl+C → SIGINT, Ctrl+\ → SIGQUIT, Ctrl+Z → SIGTSTP), or by the kernel (SIGSEGV, SIGPIPE, SIGCHLD...).
- Each signal has an action: default (Term / Core / Ign / Stop / Cont), ignore (`SIG_IGN`), or catch with a handler.

| Signal | Number | Default | Notes |
| --- | --- | --- | --- |
| SIGHUP | 1 | Term | terminal hung up; daemons often use it to reload config |
| SIGINT | 2 | Term | Ctrl+C |
| SIGQUIT | 3 | Core | Ctrl+\ |
| SIGKILL | 9 | Term | **cannot be caught, blocked or ignored** |
| SIGSEGV | 11 | Core | invalid memory access |
| SIGPIPE | 13 | Term | write to a pipe/socket with no reader left |
| SIGALRM | 14 | Term | `alarm()` |
| SIGTERM | 15 | Term | default of the `kill` command, a "polite" request to exit |
| SIGCHLD | 17 | Ign | a child finished/stopped |
| SIGCONT | 18 | Cont | |
| SIGSTOP | 19 | Stop | **cannot be caught, blocked or ignored** |
| SIGTSTP | 20 | Stop | Ctrl+Z, can be caught |

- Prefer `sigaction` over `signal` (well-defined behavior, `sa_mask`, `SA_RESTART`, `SA_SIGINFO`).
- A handler should only call **async-signal-safe** functions (`write`, `_exit`...), **not** `printf` or `malloc`. Variables shared with a handler: `volatile sig_atomic_t`.
- Blocking (`sigprocmask`) leaves a signal pending. Standard signals are not queued (one pending bit); real-time signals (SIGRTMIN–SIGRTMAX) are queued.
- fork: the child inherits handlers and the mask; pending signals are cleared. exec: handlers reset to default (ignored signals stay ignored).
- A slow syscall interrupted by a handler returns -1/EINTR unless `SA_RESTART` is set.

### IPC mechanisms
| Mechanism | Properties |
| --- | --- |
| **Pipe** (`pipe()`) | one-way byte stream, only between related processes; the shell's pipe symbol |
| **FIFO / named pipe** (`mkfifo`) | like a pipe but with a name in the filesystem, any processes |
| **Message queue** (POSIX `mq_*`, System V `msg*`) | keeps message boundaries, has priorities/types |
| **Shared memory** (POSIX `shm_open`+`mmap`, System V `shmget`) | **fastest** (no copying through the kernel) but you must synchronize yourself (semaphore/mutex) |
| **Semaphore** (POSIX `sem_*`, System V `semget`) | synchronization, carries no data |
| **Socket** (UNIX domain / TCP / UDP) | two-way, can go over a network |
| **Signal** | only signals an event, carries almost no data |
| **mmap'ed file** | several processes map the same file with `MAP_SHARED` |

- Pipe: `fd[0]` reads, `fd[1]` writes. Close the ends you do not use; as long as a write end is open, read never sees EOF. Default capacity is 64 KiB on Linux; writes ≤ PIPE_BUF (4096) bytes are atomic.
- How the shell runs `a | b`: `pipe()` → fork twice → child 1 does `dup2(fd[1], 1)`, child 2 does `dup2(fd[0], 0)` → exec.
- List System V IPC objects: `ipcs`, remove: `ipcrm`. POSIX: `/dev/shm`, `/dev/mqueue`.
- Sockets: server `socket → bind → listen → accept → read/write`; client `socket → connect`. TCP is reliable, ordered and connection-oriented; UDP is connectionless, keeps datagram boundaries, may lose/reorder.

## Quiz

### Which signals can NOT be caught, blocked or ignored? (select all that apply)
- [x] SIGKILL
- [ ] SIGTERM
- [x] SIGSTOP
- [ ] SIGINT
> SIGKILL (9) and SIGSTOP (19). SIGTERM, SIGINT and SIGTSTP can all be caught.

### Which signal does `kill 1234` (no signal given) send?
- [ ] SIGKILL
- [x] SIGTERM
- [ ] SIGINT
- [ ] SIGHUP
> The default is SIGTERM (15), giving the program a chance to clean up. `kill -9` is SIGKILL.

### Pressing Ctrl+C in a terminal sends which signal to the foreground process group?
- [x] SIGINT
- [ ] SIGTERM
- [ ] SIGQUIT
- [ ] SIGTSTP
> Ctrl+\ → SIGQUIT (creates a core dump), Ctrl+Z → SIGTSTP.

### Which function is safe to call in a signal handler?
- [ ] printf
- [ ] malloc
- [x] write
- [ ] fopen
> Only async-signal-safe functions (listed in `man 7 signal-safety`). printf and malloc use internal locks/buffers → possible deadlock or corruption.

### How should a variable shared between a signal handler and the main program be declared?
- [ ] int
- [ ] static int
- [x] volatile sig_atomic_t
- [ ] atomic_flag
> volatile so the compiler does not cache it, sig_atomic_t so reads/writes cannot be interrupted halfway.

### When is SIGCHLD sent?
- [ ] When the parent exits
- [x] When a child process terminates (or stops/continues)
- [ ] When fork is called
- [ ] When the child calls exec
> The default action is to ignore it. Catch SIGCHLD and call waitpid(-1, ..., WNOHANG) in a loop to reap zombies.

### SIGUSR1 is sent five times while it is blocked, then unblocked. How many times does the handler run?
- [ ] 5
- [x] 1
- [ ] 0
- [ ] Undefined
> Standard signals have a single pending bit and are not queued. Real-time signals are queued.

### What does a process get when it writes to a pipe whose read ends are all closed?
- [ ] read returns 0
- [x] SIGPIPE (if SIGPIPE is ignored, write returns -1 with errno = EPIPE)
- [ ] write blocks forever
- [ ] SIGCHLD
> The default action of SIGPIPE is to terminate the process.

### Which IPC mechanism is fastest for large amounts of data between two processes on the same machine?
- [ ] Pipe
- [ ] Message queue
- [x] Shared memory
- [ ] TCP socket
> Both processes access the same memory directly, with no copying through the kernel; in exchange they must synchronize themselves.

### What is the main difference between a pipe and a FIFO (named pipe)?
- [ ] A FIFO is two-way, a pipe is one-way
- [x] A FIFO has a name in the filesystem, so unrelated processes can use it
- [ ] A FIFO stores data on disk
- [ ] A pipe is much faster
> A FIFO is created with `mkfifo`; `ls -l` shows file type `p`. The data still only goes through the kernel, never to disk.

### After `int fd[2]; pipe(fd);`, which end is for reading?
- [x] fd[0]
- [ ] fd[1]
- [ ] Both
- [ ] Depends on the OS
> fd[0] reads, fd[1] writes (mnemonic: 0 is stdin, 1 is stdout).

### Why does a process reading a pipe hang forever instead of getting EOF?
- [ ] Because the pipe is full
- [x] Because a write end is still open (often the reader itself forgot close(fd[1]))
- [ ] Because fflush was not called
- [ ] Because SIGPIPE is missing
> read only returns 0 when **every** write fd of the pipe is closed, including its own copy after fork.

### What does `dup2(fd, 1)` do?
- [ ] Closes fd and reopens it as number 1
- [x] Makes fd 1 (stdout) refer to the same file as fd
- [ ] Copies the contents of file fd to stdout
- [ ] Creates a new pipe
> The shell uses dup2 for `>` redirection and `|` pipes. `2>&1` corresponds to `dup2(1, 2)`.

### How does a message queue differ from a pipe? (select all that apply)
- [x] It keeps the boundaries between messages
- [x] It supports priorities (POSIX) or filtering by type (System V)
- [ ] The two processes must be related
- [x] It lives in the kernel until removed, even when no process has it open
> A pipe is a byte stream that disappears when every fd is closed.

### Which socket type keeps message boundaries and does not guarantee order/delivery?
- [ ] SOCK_STREAM (TCP)
- [x] SOCK_DGRAM (UDP)
- [ ] UNIX domain stream
- [ ] Pipe
> TCP is a reliable, ordered byte stream. UDP sends independent datagrams.

### What is the call order on a TCP server?
- [ ] socket → connect → listen → accept
- [x] socket → bind → listen → accept
- [ ] bind → socket → accept → listen
- [ ] socket → listen → bind → connect
> Client: socket → connect. accept returns a new fd for each connection.

### Which command lists System V IPC objects (shared memory, semaphores, message queues)?
- [ ] lsof -i
- [x] ipcs
- [ ] ps -ipc
- [ ] netstat -x
> Remove them with `ipcrm`. (Newer util-linux also has lsipc.) POSIX shared memory lives in /dev/shm.
