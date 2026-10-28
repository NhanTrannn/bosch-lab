# Processes, virtual memory and scheduling

## Theory

### Processes and threads
- **Program**: a file on disk. **Process**: a running program with its own address space, PID, fd table and bookkeeping in the **PCB** (Linux: `task_struct`).
- States (`ps` STAT column): **R** running/runnable, **S** interruptible sleep, **D** uninterruptible sleep (usually waiting for I/O), **T** stopped, **Z** zombie. Extra flags: `+` foreground group, `s` session leader, `l` multithreaded, `<` high priority, `N` low priority.
- Five-state model: New → Ready → Running → Waiting (Blocked) → Terminated.

### Creating and ending processes
- `fork()`: returns 0 in the child, the child's PID in the parent, -1 on error. The child is a copy (copy-on-write) and inherits fds, signal handlers, environment variables and cwd. n consecutive forks → 2ⁿ processes.
- `exec*()`: replaces the program in the current process, keeping the PID and fds (except fds with `FD_CLOEXEC`). Only returns on error. Suffixes: **l** argument list, **v** array, **p** search PATH, **e** pass an environment.
- `wait()/waitpid()`: wait for a child to finish and get its status (`WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`, `WTERMSIG`).
- `exit()` flushes stdio buffers and runs `atexit` handlers; `_exit()` exits immediately (use it in a child after fork).
- **Zombie**: the child has died but the parent has not called `wait` → it still holds an entry (PID) in the process table. **Orphan**: the parent died first → the child is adopted by init/systemd (or a subreaper).
- The exit code is only 8 bits (0–255); killed by signal n → the shell reports 128 + n.

### Virtual memory
- Each process has its own virtual address space; the MMU translates virtual → physical addresses through the **page table**, cached in the **TLB**. Pages are usually 4 KiB.
- **Page fault**: accessing a page that is not in RAM → the kernel loads it (minor: the page is already in RAM but not mapped; major: it must be read from disk). Accessing an invalid address → SIGSEGV.
- **Swap**: moving rarely used pages to disk. **Thrashing**: constant swapping that makes the system crawl.
- **Copy-on-write**: fork shares pages read-only and copies a page only when someone writes to it.
- Benefits: process isolation, programs bigger than RAM, shared libraries, mmap'ed files.
- User space / kernel space: user programs enter the kernel through **system calls** (a CPU mode switch, e.g. the `syscall` instruction on x86_64). Interrupts and exceptions also enter the kernel.
- Fragmentation: **external** (enough free memory in total, but not contiguous — paging solves it), **internal** (allocating more than needed inside a block/page).

### Scheduling
- Context switch: save registers and PC (and switch page tables if it is another process) → it has a cost; switching between threads of the same process is cheaper.
- Algorithms: FCFS, SJF (optimal average waiting time, needs to know job lengths), Round Robin (time slice), Priority (can starve → aging), Multilevel feedback queue.
- Preemptive (the OS can interrupt a task) vs non-preemptive (tasks yield on their own).
- Linux: CFS for normal tasks (`nice` from -20 to 19, lower = higher priority); real-time `SCHED_FIFO`, `SCHED_RR` (priority 1–99) always run before normal tasks. Linux 6.6+ replaced CFS with EEVDF.
- **Priority inversion**: a high-priority task waits for a lock held by a low-priority task while a medium-priority task hogs the CPU. Fixes: priority inheritance, priority ceiling (the Mars Pathfinder incident).

### Useful commands
`ps -ef`, `ps aux`, `ps -o pid,ppid,stat,cmd`, `pstree -p`, `top`/`htop`, `kill`, `nice`/`renice`, `jobs`/`fg`/`bg`, `&`, `nohup`, `/proc/<pid>/` (status, maps, fd, cmdline), `strace` (shows system calls), `free -h`, `vmstat`.

## Quiz

### What does `fork()` return in the child process?
- [x] 0
- [ ] The child's PID
- [ ] The parent's PID
- [ ] -1
> The parent gets the child's PID, the child gets 0, and an error returns -1 (in the parent; no child is created).

### How many times does this print "hello"?
```c
fork(); fork(); fork();
printf("hello\n");
```
- [ ] 3
- [ ] 6
- [x] 8
- [ ] 4
> Each fork doubles the number of processes: 2³ = 8.

### What is a zombie process?
- [ ] A process whose parent has died
- [x] A process that has finished but whose parent has not called wait() to collect its status
- [ ] A process stuck in a deadlock
- [ ] A process in the stopped state
> A zombie (STAT Z, `<defunct>`) uses no CPU/RAM but holds a PID. kill -9 cannot remove it; the parent must wait, or the parent must die.

### When a parent dies before its child, the child:
- [ ] Is killed too
- [ ] Becomes a zombie
- [x] Is adopted by init/systemd (or a subreaper process)
- [ ] Is stopped
> That is an orphan process. Its PPID becomes 1 or the PID of a subreaper (e.g. `systemd --user`).

### After a successful `execvp()`, what does NOT change?
- [ ] The code segment
- [ ] The heap
- [x] The PID
- [ ] The stack
> exec replaces the whole image but keeps the PID, PPID, open fds (except close-on-exec ones) and cwd.

### What does state D in `ps` mean?
- [ ] Dead
- [x] Uninterruptible sleep (usually waiting for I/O); signals do not wake it
- [ ] Daemon
- [ ] Detached
> A process in D cannot be killed until the I/O finishes. Many D processes usually mean a disk/NFS problem.

### What is the benefit of copy-on-write in fork()?
- [ ] Parent and child share variables and see each other's changes
- [x] fork is fast because memory is not copied up front; a page is copied only when written
- [ ] It stops the child from writing to memory
- [ ] It reduces the number of PIDs used
> Especially useful for fork followed immediately by exec (almost no page is ever copied).

### Which function should a child use to exit after fork if it must not flush the parent's stdio buffers again?
- [ ] exit()
- [x] _exit()
- [ ] return
- [ ] abort()
> exit() flushes the inherited stdio buffers → output is printed twice. _exit() exits without flushing.

### What exit code does the shell report when a process is killed by SIGKILL (9)?
- [ ] 9
- [ ] 1
- [x] 137
- [ ] 255
> 128 + the signal number = 137. SIGINT (2) gives 130, SIGSEGV (11) gives 139.

### What is the TLB?
- [ ] A table of running processes
- [x] A hardware cache of recent virtual → physical address translations
- [ ] The hard disk's cache
- [ ] The interrupt vector table
> A TLB miss forces a page-table walk. A context switch between processes may flush the TLB (unless ASID/PCID is available).

### When does a page fault happen?
- [ ] When a program divides by zero
- [x] When accessing a virtual page that is not mapped to a physical frame in RAM
- [ ] When RAM has a hardware fault
- [ ] When the stack overflows
> It is not an error: the kernel loads the page and continues. Only an invalid address leads to SIGSEGV.

### Which scheduling algorithm can cause starvation?
- [ ] Round Robin
- [x] Priority scheduling without aging
- [ ] FCFS
- [ ] None of them
> Low-priority tasks can wait forever. Aging raises priority over waiting time. SJF can also starve long jobs.

### What is priority inversion?
- [ ] The OS reverses the priority of every task
- [x] A high-priority task is blocked by a low-priority task holding a resource, while a medium-priority task takes the CPU
- [ ] A low-priority task runs first because of FCFS
- [ ] Two tasks of equal priority wait for each other
> Fixes: priority inheritance (the lock holder temporarily gets the higher priority) or priority ceiling.

### Which `nice` value gives the highest priority to a normal task on Linux?
- [x] -20
- [ ] 0
- [ ] 19
- [ ] 99
> nice ranges from -20 (high priority) to 19 (low). Only root can go below 0. 1–99 is the real-time priority scale.

### A context switch between two threads of the same process is cheaper than between two processes because:
- [ ] Threads have no registers of their own
- [x] The address space (page table) does not change and the TLB is flushed less
- [ ] Threads do not need to save the PC
- [ ] The kernel is not involved
> Registers, the stack pointer and the PC still have to be saved/restored.

### What is a system call?
- [ ] A C library function running in user space
- [x] The mechanism a user program uses to ask the kernel for a service, switching the CPU into kernel mode
- [ ] A call between two processes
- [ ] A hardware interrupt from a device
> E.g. read, write, open, fork. printf is a library function that calls the write syscall internally. Watch them with `strace`.

### Which command shows the process tree with PIDs?
- [ ] ps -t
- [x] pstree -p
- [ ] top -tree
- [ ] ls /proc
> `ps -ef --forest` or `ps axjf` also work.

### Where does Linux expose runtime information about process 1234?
- [ ] /var/run/1234
- [x] /proc/1234/
- [ ] /sys/process/1234
- [ ] /dev/1234
> /proc is a virtual filesystem: status, maps (memory regions), fd/ (open fds), cmdline, environ.
