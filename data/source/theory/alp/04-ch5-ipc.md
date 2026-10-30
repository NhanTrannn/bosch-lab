---
tags: [linux, alp, ipc, shared-memory, pipes, sockets]
---

# ALP ch.5: Interprocess communication (IPC)

## Theory

Follows *Advanced Linux Programming*, chapter 5 "Interprocess Communication". Labs: `alp/12` … `alp/15`; see also `linux-ipc/*`. The "Signals and IPC" topic above has a comparison table.

The book covers five kinds of IPC: **shared memory**, **mapped memory**, **pipes**, **FIFOs** and **sockets**, plus **process semaphores** for synchronization. Three ways to classify them:
- Whether they are limited to **related** processes (parent/child) — pipes are.
- Whether they are read-only or write-only, or both directions.
- How many processes can take part.

### Shared memory, System V (5.1)
- The **fastest** IPC: processes access the same physical memory, reading and writing without system calls or copies. But the kernel does **no synchronization** → use semaphores/locks yourself.
- Model: one process **allocates** a segment, the others **attach** to it. A segment is a multiple of the **page size** (`getpagesize()`, usually 4 KB).
- `shmget(key, size, flags)`: create or get a segment. key = `IPC_PRIVATE` → always a new segment. flags: `IPC_CREAT` (create if missing), `IPC_EXCL` (with IPC_CREAT: fail if it already exists), permissions `S_IRUSR | S_IWUSR`. Returns the **segment id**.
- `shmat(id, addr, flags)`: attach it to the address space (addr = NULL lets the kernel choose), returns the address (`SHM_RDONLY` = read-only). After fork the child inherits attached segments. `shmdt(addr)`: detach. exit/exec detach automatically.
- `shmctl(id, IPC_STAT, &ds)`: information (`shm_segsz`, `shm_nattch` = number of current attachments). `shmctl(id, IPC_RMID, NULL)`: mark for deletion; the segment is destroyed when the last process detaches.
- A segment does **not disappear** when every process has exited. You must IPC_RMID it, otherwise it leaks until reboot. Commands: `ipcs -m` (list), `ipcrm shm <id>` (old syntax) or `ipcrm -m <id>`.
- Pros/cons: fast, many processes can share; but you must synchronize yourself, agree on a key and a data layout, and orphaned segments are easy to leave behind.

### Process semaphores, System V (5.2)
- Allocated as **sets** of semaphores: `semget(key, nsems, flags)` → an id. Delete with `semctl(id, 0, IPC_RMID)` (you must delete it; it does not go away by itself). Commands: `ipcs -s`, `ipcrm sem <id>`.
- Initialize with `semctl(id, 0, SETALL, arg)` or `SETVAL`; you must declare `union semun { int val; struct semid_ds *buf; unsigned short *array; };` yourself. A new semaphore's value is undefined, so always initialize it.
- Operations: `semop(id, ops, nops)` with an array of `struct sembuf { sem_num, sem_op, sem_flg }`. Positive `sem_op` = post (add), negative = wait (block until it can subtract without going negative), 0 = wait until the value is 0. All operations in one semop call are **atomic**.
- `sem_flg`: `IPC_NOWAIT` (do not block, return EAGAIN), **`SEM_UNDO`** (the kernel undoes the change when the process exits, so a semaphore does not stay stuck when a process dies inside a critical section).
- (POSIX semaphores `sem_open`/`sem_wait` have a simpler API; see `linux-ipc`.)

### Mapped memory (5.3)
- `mmap` maps a **file** into memory: read and write the file like an array, without read/write calls. Several processes mapping the same file share the data → IPC (backed by a file, so the data outlives the processes).
- `mmap(addr, length, prot, flags, fd, offset)`: addr is usually NULL; offset is a multiple of the page size. prot: `PROT_READ`, `PROT_WRITE`, `PROT_EXEC`. flags: `MAP_SHARED` (writes go to the file and other processes see them) **or** `MAP_PRIVATE` (copy-on-write: writes are only seen by this process and are not written to the file), `MAP_FIXED` (must use exactly addr, page-aligned). Errors return `MAP_FAILED`. `munmap(addr, length)` unmaps it (exit unmaps too).
- The file must be long enough before mapping: the book does `lseek(fd, FILE_LENGTH + 1, SEEK_SET); write(fd, "", 1);` (today `ftruncate` is usual). Accessing past the end of the file → **SIGBUS**.
- `msync(addr, length, flags)`: push changes to disk. `MS_ASYNC` (schedule the write and return immediately), `MS_SYNC` (wait until written), `MS_INVALIDATE` (invalidate other mappings so they re-read the file). Linux may keep changes in buffers, so msync when you must be sure the data is on disk.
- Sharing through mmap also needs synchronization (semaphores or `fcntl` file locks).
- Other uses: mapping `/dev/zero` (or `MAP_ANONYMOUS`) gives zero-filled memory that parent and child share after fork; fast file reading.

### Pipes (5.4)
- **One-way**: `int fds[2]; pipe(fds);` → `fds[0]` is the **read end**, `fds[1]` the **write end**. Data flows in FIFO order. Only usable between **related** processes (create the pipe, then fork; the child inherits the fds).
- Limited capacity: writing to a full pipe blocks; reading an empty pipe (with a write end still open) blocks. When all write ends are closed, read returns 0 (**EOF**). Writing when all read ends are closed → **SIGPIPE** (or EPIPE).
- Each process must **close the ends it does not use**. Otherwise the reader never sees EOF (because it still holds a write end itself).
- `fdopen(fd, "r"/"w")` wraps an fd in a `FILE*` so you can use fprintf/fgets (listing 5.7). Remember fflush or fclose so the data actually enters the pipe.
- **Redirection** (listing 5.8): in the child, `dup2(fds[0], STDIN_FILENO)` and then exec `sort` → sort reads the pipe as its stdin. `dup2(old, new)` closes new (if open) and makes new refer to the same file as old.
- **`popen(command, "r"/"w")`** (listing 5.9): pipe + fork + exec `sh -c command`; with "w" you write to the command's stdin, with "r" you read its stdout. **`pclose`** closes the pipe, waits for the child and returns its wait status. It has the same security risks as system().
- Writes of at most **PIPE_BUF** bytes (4 KB on Linux) are **atomic**: data from several processes writing at once is not interleaved within such a small write.

### FIFOs (5.4.5)
- A **named pipe**: a pipe with a name in the filesystem (`ls -l` shows type `p`). **Unrelated** processes can use it too. Create it with the command `mkfifo /tmp/fifo` or the function `mkfifo(path, mode)`.
- Open it like a normal file: opening for reading blocks until some process opens it for writing, and vice versa. Several processes may write; writes ≤ PIPE_BUF bytes are atomic.
- Unlike Windows named pipes, Linux FIFOs only work on one machine and are one-way (not client–server).

### Sockets (5.5)
- A **two-way** channel, usable between processes on the same machine or **over a network**.
- Parameters of `socket(namespace, style, protocol)`:
  - **Namespace**: `PF_LOCAL`/`PF_UNIX` (same machine, the address is a file path), `PF_INET` (IPv4: IP address + port).
  - **Style**: `SOCK_STREAM` (like a phone call: connected, ordered, no loss, like TCP), `SOCK_DGRAM` (like mailing letters: separate packets that may be lost or reordered, like UDP).
  - **Protocol**: usually 0 (the default protocol).
- System calls: `socket` (create), `close` (destroy), `connect` (a client connects to a server), `bind` (give a server socket an address), `listen` (accept connections, with a queue length), `accept` (take one connection, returning a **new socket** for it; the listening socket keeps accepting other clients).
- Server: socket → bind → listen → accept (loop) → read/write → close. Client: socket → connect → write/read → close. Use `read`/`write` (or `send`/`recv`) like with files.
- **Local sockets** (listings 5.10, 5.11): `struct sockaddr_un`, `sun_family = AF_LOCAL`, `sun_path` = a path (at most 108 bytes). The socket file must be `unlink`ed when done. The process needs write permission on the directory to create the socket.
- **Internet sockets** (listing 5.12): `struct sockaddr_in`, `sin_family = AF_INET`, `sin_port = htons(80)`, `sin_addr`. Resolve host names with `gethostbyname` (today `getaddrinfo`). **Byte order**: numbers on the network are big-endian ("network byte order") → `htons`/`htonl` (host to network), `ntohs`/`ntohl`. Only root can bind ports below 1024. Standard services have fixed ports (`/etc/services`: HTTP 80...).
- **`socketpair(PF_LOCAL, SOCK_STREAM, 0, fds)`**: creates a pair of already-connected sockets, like a pipe but **two-way** (each fd both reads and writes). Only for the local namespace and related processes.

## Quiz

### Which IPC mechanism does the book call the fastest? {#alp-ch5-001}
- [ ] Pipe
- [ ] Socket
- [x] Shared memory
- [ ] FIFO
> Once attached, reading and writing are ordinary memory accesses with no system calls or copies through the kernel. The price is that you must synchronize yourself.

### What does `shmget(IPC_PRIVATE, size, IPC_CREAT | 0600)` do? {#alp-ch5-002}
- [ ] Gets the system-wide shared segment
- [x] Always creates a new segment; other processes can only use it if they know the id (e.g. a child after fork)
- [ ] Creates a segment usable only by threads
- [ ] Fails because the key is missing
> IPC_PRIVATE guarantees it does not clash with an existing segment. Unrelated processes that want to share must agree on a key (usually made with `ftok`).

### Every process using a shared memory segment has exited without calling IPC_RMID. What happens to the segment? {#alp-ch5-003}
- [ ] It is deleted when the last process exits
- [x] It stays in the system (visible with `ipcs -m`) until removed with IPC_RMID/ipcrm or a reboot
- [ ] It is written to disk
- [ ] It becomes a file in /tmp
> A common resource leak with System V IPC. Semaphore sets behave the same way.

### Which function attaches a shared memory segment to a process's address space? {#alp-ch5-004}
- [ ] `shmget`
- [x] `shmat`
- [ ] `shmctl`
- [ ] `mmap`
> shmget creates/gets the id; shmat attaches and returns the address; shmdt detaches; shmctl gets information or deletes.

### What does `shm_nattch` in `struct shmid_ds` tell you? {#alp-ch5-005}
- [ ] The segment's size
- [x] How many times the segment is currently attached
- [ ] The creator's PID
- [ ] The number of related semaphores
> After IPC_RMID, the segment is only really destroyed when nattch reaches 0.

### What size should a shared memory segment be? {#alp-ch5-006}
- [ ] A prime number
- [x] A multiple of the page size (getpagesize(), usually 4096)
- [ ] Exactly sizeof the data
- [ ] A power of 10
> The kernel allocates whole pages; asking for 100 bytes still takes a page.

### What is the `SEM_UNDO` flag in semop for? {#alp-ch5-007}
- [ ] Undoing the semop just performed, immediately
- [x] The kernel undoes the process's changes when it exits, so the semaphore does not get stuck if the process dies inside a critical section
- [ ] Allowing negative values
- [ ] Not blocking when the value is too low
> Not blocking is IPC_NOWAIT.

### In semop, what does `sem_op = -1` mean? {#alp-ch5-008}
- [ ] Delete the semaphore
- [x] Wait (P): subtract 1, blocking if the current value is 0
- [ ] Post (V): add 1
- [ ] Wait until the value is 0
> A positive sem_op is a post; sem_op = 0 waits until the semaphore is 0.

### What is the main difference between MAP_SHARED and MAP_PRIVATE? {#alp-ch5-009}
- [ ] MAP_SHARED is read-only, MAP_PRIVATE is read-write
- [x] MAP_SHARED writes go to the file and other processes mapping it see them; MAP_PRIVATE is copy-on-write, so writes are only seen by that process
- [ ] MAP_PRIVATE is faster because there is no page cache
- [ ] MAP_SHARED only works with /dev/zero
> The two flags are mutually exclusive; exactly one must be chosen. IPC through mmap uses MAP_SHARED.

### What does mmap return on failure? {#alp-ch5-010}
- [ ] NULL
- [ ] -1 as an int
- [x] `MAP_FAILED` (i.e. `(void *)-1`)
- [ ] 0
> Check `if (p == MAP_FAILED)`, not NULL.

### Before mmap'ing a newly created file, why does the book `lseek` past the end and `write` one byte? {#alp-ch5-011}
- [ ] To open the file for writing
- [x] To make the file long enough; accessing the mapping past the end of the file causes SIGBUS
- [ ] To erase the old contents
- [ ] Because mmap only works on non-empty files
> The modern way: `ftruncate(fd, length)`.

### What does `msync(addr, len, MS_SYNC)` do? {#alp-ch5-012}
- [ ] Synchronizes two processes
- [x] Writes the mapping's changes to the file on disk and waits until they are written
- [ ] Locks the memory
- [ ] Unmaps it
> MS_ASYNC only schedules the write and returns immediately; MS_INVALIDATE makes other mappings re-read the new data.

### After `pipe(fds)`, which end is for reading? {#alp-ch5-013}
- [x] `fds[0]`
- [ ] `fds[1]`
- [ ] Both
- [ ] Whichever calls read first
> Mnemonic: 0 is like stdin (read), 1 like stdout (write).

### A child reading a pipe never gets EOF even though the parent closed its write end. A common cause? {#alp-ch5-014}
- [ ] The pipe is full
- [x] The child itself (or another process) still holds an open write-end fd
- [ ] The parent must send a '\0'
- [ ] You must use a FIFO
> EOF only appears when every fd referring to the write end is closed. After fork, each process must close the end it does not use.

### What happens when writing to a pipe whose read ends are all closed? {#alp-ch5-015}
- [ ] The write succeeds and the data is dropped
- [ ] It blocks forever
- [x] The process gets SIGPIPE (killed by default); if SIGPIPE is ignored, write returns -1 with errno = EPIPE
- [ ] It returns 0
> E.g. `yes | head -1`: head exits, yes gets SIGPIPE and terminates.

### Which function wraps a file descriptor in a `FILE *` so you can use fprintf/fgets? {#alp-ch5-016}
- [ ] `fopen`
- [x] `fdopen`
- [ ] `fileno`
- [ ] `freopen`
> fileno does the opposite: gets the fd from a FILE*.

### For `sort` (run with exec in a child) to read data from a pipe, what must the child do before exec? {#alp-ch5-017}
- [ ] `dup2(STDIN_FILENO, fds[0])`
- [x] `dup2(fds[0], STDIN_FILENO)`, close the extra pipe fds, then exec
- [ ] `close(STDIN_FILENO)` is enough
- [ ] Pass fds[0] as a command-line argument
> `dup2(old, new)` makes new refer to the same file as old. fds survive exec, so sort reading fd 0 reads the pipe.

### What does `popen("sort", "w")` return? {#alp-ch5-018}
- [ ] sort's PID
- [x] A `FILE *` for writing to sort's stdin
- [ ] A `FILE *` for reading sort's output
- [ ] The pipe's fd
> Close it with `pclose`, not fclose. pclose waits for the command to finish and returns its wait status.

### Which statements about FIFOs (named pipes) are TRUE? (select all that apply) {#alp-ch5-019}
- [x] They have a name in the filesystem and are created with mkfifo
- [x] Unrelated processes can use them
- [x] Opening for reading blocks until some process opens it for writing
- [ ] The data is kept on disk after the processes exit
> The data goes through the kernel and is never stored on disk; the FIFO file is just a named meeting point.

### Up to how many bytes is a write to a pipe/FIFO guaranteed to be atomic (not interleaved with other processes' data)? {#alp-ch5-020}
- [ ] Any size
- [x] Up to PIPE_BUF bytes (4 KB on Linux)
- [ ] Up to 1 byte
- [ ] Up to 64 KB
> Writes larger than PIPE_BUF may be split and interleaved with other writers' data.

### How do SOCK_STREAM and SOCK_DGRAM differ? {#alp-ch5-021}
- [x] STREAM is connected, ordered and lossless (like TCP); DGRAM sends independent packets that may be lost or reordered (like UDP)
- [ ] STREAM is for files, DGRAM for networks
- [ ] STREAM is one-way, DGRAM two-way
- [ ] No difference except the name
> The book compares SOCK_STREAM to a phone call and SOCK_DGRAM to mailing letters.

### What is the correct call order on a SOCK_STREAM server? {#alp-ch5-022}
- [ ] socket → connect → accept
- [ ] socket → listen → bind → accept
- [x] socket → bind → listen → accept
- [ ] bind → socket → accept → listen
> Client: socket → connect. accept returns a new socket for each connection.

### What does `accept` return? {#alp-ch5-023}
- [ ] 0 on success
- [x] A new socket dedicated to the accepted connection; the listening socket stays usable
- [ ] The client's IP address
- [ ] The original listening socket
> Servers usually loop on accept and handle each connection (possibly forking or creating a thread per connection).

### Why call `htons(port)` when setting `sin_port`? {#alp-ch5-024}
- [ ] To validate the port
- [x] To convert the number from the machine's byte order (x86: little-endian) to network byte order (big-endian)
- [ ] To encrypt the port
- [ ] Because sin_port is a string
> htons/htonl (host to network short/long), ntohs/ntohl (the other way). On big-endian machines these functions do nothing.

### What is the address of a local socket (PF_LOCAL)? {#alp-ch5-025}
- [ ] IP 127.0.0.1 and a port
- [x] A path in the filesystem (sun_path), which should be unlinked when done
- [ ] The server's PID
- [ ] The host name
> Local sockets only work on one machine and are faster than sockets over loopback.

### Which ports can only a root process bind? {#alp-ch5-026}
- [x] Below 1024
- [ ] Above 49152
- [ ] Port 0
- [ ] Every port needs root
> Port 0 in bind means "let the kernel choose a free port".

### Which statements about socketpair are TRUE? (select all that apply) {#alp-ch5-027}
- [x] It creates two sockets that are already connected
- [x] It is two-way: each end both reads and writes
- [x] It only works with the local namespace (PF_LOCAL)
- [ ] It works between two machines over a network
> Like a two-way pipe; usually created before fork so parent and child can talk.

### Which mechanism requires the processes to be related (parent/child, common ancestor)? {#alp-ch5-028}
- [x] An (unnamed) pipe
- [ ] A FIFO
- [ ] Shared memory with a shared key
- [ ] An Internet socket
> A pipe exists only as fds, which can only be passed on through fork. FIFOs, keyed shared memory and sockets have names/addresses any process can find.
