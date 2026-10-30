# Linux: IPC — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

### 01_pipe — pipe: the parent writes, the child reads

**Question:** How many bytes does the child read? What does read return the last time? What if the child does NOT close fd[1]?

**Expected output:**

```text
Child read 17 bytes: hello from parent
Child: read returned 0
Parent: done

If the child does not close(fd[1]): after the first line the program HANGS (the child waits in read forever).
```

**Explanation:**

- `"hello from parent"` is 17 characters; `strlen` does not count the `'\0'`.
- Once the parent closes the write end, `read` returns 0 = EOF → the loop ends.
- If the child still holds `fd[1]`, the kernel sees a remaining "writer" (the child itself), so `read` blocks forever → a self-inflicted deadlock.
- Writing to a pipe with no reader left → **SIGPIPE** (deadly by default), or `write` returns `EPIPE` if the signal is ignored.

**Exam relevance:** A pipe is one-way and only works between related processes (the fds are inherited through fork). Writes ≤ `PIPE_BUF` (4096 bytes on Linux) are atomic. The default capacity is 64 KiB.

### 02_pipe_dup2 — Implementing ls / | wc -l with dup2 ⚠️ nondeterministic

**Question:** What number does the program print (compared with running ls / | wc -l in the shell)? What if the parent forgets to close fd[1]?

**Expected output:**

```text
22                                  <- exactly what "ls / | wc -l" prints on your machine
Done: same as  ls / | wc -l

If the parent does not close fd[1]: wc never sees EOF → it hangs.
```

**Explanation:**

- Child 1: stdout (fd 1) → the pipe's write end, then `exec ls`. Child 2: stdin (fd 0) ← the read end, then `exec wc -l`.
- File descriptors survive `exec`, so `ls` and `wc` have no idea they are using a pipe.
- Every copy of the write end must be closed before `wc` gets EOF: child 1 (once `ls` exits), child 2 and **the parent too**.

**Exam relevance:** `dup(fd)` returns the lowest free fd. `2>&1` in the shell is exactly `dup2(1, 2)`. The order of shell redirections matters because it is a sequence of dup2 calls.

### 03_fifo — A named pipe (FIFO) between 2 programs

**Question:** When only the reader runs, at which line does it stop? When the writer presses Ctrl+D, what does the reader print and how does it end?

**Expected output:**

```text
Terminal 1 (reader, started first):
Reader: opening the FIFO...             <- stops here until the writer opens it
Reader: opened, waiting for data
Reader got: hello
Reader got: bosch
Reader: read returned 0, exiting

Terminal 2 (writer):
Writer: opening the FIFO...
Writer: opened. Type a few lines, then Ctrl+D to finish
hello
bosch
^D
Writer: closed the FIFO
```

**Explanation:**

- Opening the read end blocks until there is a writer (and vice versa) — the default behavior without `O_NONBLOCK`.
- The writer closes its fd → the reader gets EOF (read = 0).
- `ls -l` shows `p` (pipe) as the first character. No data is written to disk; the file is just a "meeting point" in the filesystem.

**Exam relevance:** File type characters in `ls -l`: `-` regular file, `d` directory, `l` symlink, `c` char device, `b` block device, `p` FIFO, `s` socket.

### 04_shm — POSIX shared memory between a parent and a child

**Question:** Which string does the parent read in mem? What is the local variable in the parent?

**Expected output:**

```text
Child:  mem = "child wrote to shm", local = 99
Parent: mem = "child wrote to shm", local = 1
```

**Explanation:**

- Shared memory is the **fastest** IPC: once mapped, reads and writes are ordinary memory accesses with no system calls → but you must synchronize yourself (a semaphore, or a mutex with `PTHREAD_PROCESS_SHARED`).
- `local` lives in private memory, copy-on-write after `fork` → the parent still sees 1.
- POSIX shm objects live in `/dev/shm` (tmpfs). `close(fd)` does not remove the mapping; you need `munmap` and `shm_unlink`.
- Older Ubuntu (glibc < 2.34) needs `-lrt` at link time.

**Exam relevance:** System V: `shmget / shmat / shmdt / shmctl(IPC_RMID)`, listed with `ipcs -m`. POSIX: `shm_open / ftruncate / mmap / munmap / shm_unlink`.

### 05_mmap_file — Mapping a file into memory with mmap

**Question:** What does the "After" line print? How many bytes is the file?

**Expected output:**

```text
Before: hello mmap world
After : HELLO MMAP world
File size = 17 bytes
```

**Explanation:**

- 16 characters + `'\n'` = 17 bytes.
- Writing straight into the mapped memory changes the file, because the mapping is `MAP_SHARED`. With `MAP_PRIVATE` the change only lands in a private copy-on-write page and the file is unchanged.
- `mmap` works in **pages** (4096 bytes): accessing past the file but inside the last page reads 0 and is never saved; going past that page → **SIGBUS/SIGSEGV**.

**Exam relevance:** mmap's advantages: no copying between kernel and user buffers, convenient random access. `MAP_ANONYMOUS` creates memory with no file behind it (malloc uses it for large blocks).

### 06_unix_socket — UNIX domain socket: a server/client pair using fork

**Question:** In what order are the two lines printed? Why must bind/listen happen BEFORE fork?

**Expected output:**

```text
Server got: ping
Client got: pong
```

**Explanation:**

- The client only prints after receiving "pong", and the server only sends "pong" after printing → a fixed order.
- If you forked first and bound/listened afterwards, the client could `connect` before the socket is listening → `ECONNREFUSED`/`ENOENT`.
- The socket file remains after the program exits → the next `bind` fails with **Address already in use** unless you `unlink` it.
- `socketpair(AF_UNIX, ...)` creates a ready-connected pair of sockets, handy for parent–child.

**Exam relevance:** `SOCK_STREAM` (TCP-like, reliable, ordered, a byte stream) vs `SOCK_DGRAM` (UDP-like, keeps message boundaries). UNIX sockets can even pass file descriptors (SCM_RIGHTS).

### 07_mqueue — POSIX message queues and priorities

**Question:** The child sends 3 messages with priorities 1, 5 and 3. In what order does the parent receive them? What is mq_curmsgs before receiving?

**Expected output:**

```text
messages waiting: 3
received "high" (prio 5)
received "medium" (prio 3)
received "low" (prio 1)
```

**Explanation:**

- The child has exited but the 3 messages are still in the queue, because the queue is a kernel object independent of the sending process.
- `mq_receive` always takes the highest-priority message → 5, 3, 1.
- Each receive takes exactly one whole message (boundaries are kept), unlike a pipe, which is a continuous byte stream.

**Exam relevance:** POSIX mq (`mq_open`, names start with `/`, link with `-lrt` on old glibc) and System V mq (`msgget`, `msgsnd`, `msgrcv`, selecting messages by `mtype`). Inspect with `ls /dev/mqueue` and `ipcs -q`.

### 08_sigpipe — Writing to a pipe with no reader: SIGPIPE and EPIPE

**Question:** With SIGPIPE ignored, what does write return? With the default action, how does the child end? What is the default pipe capacity on Linux?

**Expected output:**

```text
pipe capacity = 65536 bytes
SIGPIPE ignored: write = -1, errno = Broken pipe
child killed by signal 13 (Broken pipe)
1st read = 2, 2nd read = 0 (EOF)
```

**Explanation:**

- A Linux pipe holds 64 KiB by default; when it is full, write blocks (or returns EAGAIN with O_NONBLOCK).
- The read end is closed: with SIGPIPE ignored, write returns -1/EPIPE.
- The child restores SIG_DFL → it gets SIGPIPE (13) and dies inside write, so the "not printed" line never runs.
- Reading after every write end is closed: the remaining data (2 bytes) comes first, then 0 = EOF.

**Exam relevance:** Network servers usually do `signal(SIGPIPE, SIG_IGN)` or use `send(..., MSG_NOSIGNAL)` so they do not die when a client disconnects. Always close unused pipe ends after fork.
