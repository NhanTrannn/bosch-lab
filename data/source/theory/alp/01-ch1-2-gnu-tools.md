---
tags: [linux, alp, gcc, make, gdb, libraries]
---

# ALP ch.1–2: GNU tools and writing good Linux software

## Theory

Follows *Advanced Linux Programming* (Mitchell, Oldham, Samuel), chapter 1 "Getting Started" and chapter 2 "Writing Good GNU/Linux Software". Labs: `alp/01` … `alp/06`. See also the "Build, link and debug" topic.

### Compiling with GCC (1.2)
- `gcc` for C, `g++` for C++. Compile one file to an object: `gcc -c main.c` → `main.o`. Link: `g++ -o reciprocal main.o reciprocal.o` (link with g++ when there is C++ code, so libstdc++ is pulled in).
- Common options: `-I dir` (add a header search directory), `-D NAME=value` (define a macro, e.g. `-DNDEBUG` disables assert), `-O2` (optimize), `-g` (debug info), `-o file` (output name), `-Wall` (warnings), `-L dir` and `-l name` (library directory and name: `-lm` → `libm.so`/`libm.a`), `-static`, `-shared`, `-fPIC`, `-pg` (gprof).
- Optimization (`-O2`) makes debugging harder (variables optimized away, code reordered); debug with `-g` and no optimization.

### GNU Make (1.3)
- A Makefile consists of **rules**: `target: dependencies`, then command lines starting with a **TAB**. Make only rebuilds a target when a dependency is **newer** than the target (comparing modification times).
- The default target is the first one. Phony targets like `clean` produce no file (declare them `.PHONY`).
- Variables: `CFLAGS = -O2`; override from the command line: `make CFLAGS=-g`. Make has built-in implicit rules to build `.o` from `.c`.
- `make -j4`: build in parallel; `make -n`: print the commands without running them.

### GDB (1.4)
- Compile with `-g`. `gdb ./prog` → `run args` → after a crash: `where`/`backtrace` (show the call stack), `up`/`down` (move between frames), `print expr`.
- `break main` / `break file.c:20`, `next` (run the next line, stepping over calls), `step` (step into calls), `continue`, `finish`, `info locals`, `quit`.
- The book's point: a crashing program built without `-g` gives a backtrace with addresses only, no line numbers.

### Finding information (1.5)
- **man**: section 1 (user commands), **2 (system calls)**, **3 (C library functions)**, 8 (admin commands). `man 2 open`, `man 3 printf`, `man sleep` vs `man 3 sleep`. `man -k keyword` (= `apropos`), `whatis`.
- **info**: GNU hypertext documentation (gcc, libc, make, emacs).
- Headers: `/usr/include`, `/usr/include/sys`, `/usr/include/bits` (architecture-specific definitions), `/usr/include/linux`. The Linux and glibc sources are the most accurate documentation.

### Interacting with the execution environment (2.1)
- `int main(int argc, char *argv[])`: `argc` = number of elements, `argv[0]` = the program name, `argv[argc]` = NULL.
- **GNU command-line conventions**: short options `-h` (can be combined, `-vo`), long options `--help`, `--output=file`. Provide `--help` and `--version`. `--` ends the options.
- **getopt_long** (`<getopt.h>`): a short-options string `"ho:v"` (`:` = needs an argument), an array of `struct option {const char *name; int has_arg; int *flag; int val;}` ending with `{NULL, 0, NULL, 0}`. Call it in a loop until it returns -1; `optarg` = the option's argument; `optind` = the index of the first non-option argument; `'?'` = an invalid option (the error goes to stderr). GNU getopt permutes argv so non-options end up last.
- **stdin/stdout/stderr** = fds 0/1/2. Error messages go to stderr so they stay separate from the output. **stdout is buffered** (line-buffered on a terminal, fully buffered on a file/pipe), **stderr is not**. `fflush(stdout)` pushes it out now. C++: `cout` is buffered, `cerr` is not; `endl` flushes, `'\n'` does not.
- Merging stderr into stdout: `prog > out.txt 2>&1` (2>&1 after a file redirection), `prog 2>&1 | filter` (before a pipe).
- **Exit code**: 0 = success, non-zero = error; the shell keeps it in `$?`. Only the low 8 bits survive → use 0–127; when killed by a signal the shell reports **128 + the signal number**.
- **Environment**: `NAME=value` pairs (upper-case names by convention): `USER`, `HOME`, `PATH`, `DISPLAY`. Shell: `printenv`, `echo $HOME`, `export EDITOR=emacs`. C: `getenv` (NULL if missing), `setenv`, `unsetenv`, iterate `extern char **environ;` (do not modify it directly). A child gets a **copy** of its parent's environment. Often used for configuration (e.g. `SERVER_NAME`, with a default when the variable is missing).
- **Temporary files** (usually in `/tmp`): avoid name collisions between instances, give only the owner access, use unpredictable names. `mkstemp("/tmp/x.XXXXXX")` → an fd, mode 0600, created with O_EXCL; `unlink` it right away if it is internal (the file disappears when the fd is closed, even on a crash). `tmpfile()` → an already-unlinked `FILE*`. **Do not** use `mktemp`, `tmpnam`, `tempnam`.

### Coding defensively (2.2)
- **assert(expr)** (`<assert.h>`): if false, prints the file, line and expression, then calls `abort()`. `-DNDEBUG` removes every assert → assert expressions **must not have side effects** (function calls, assignments, `++`). Use them for internal checks (NULL pointers, parameter conditions), not to validate user input.
- **System calls can fail** because of: exhausted resources (memory, the open-file limit), missing permissions, invalid arguments, external failures (a bad disk, the network), or being **interrupted by a signal**.
- Errors: usually -1 is returned (NULL for functions returning pointers) and **errno** is set (`<errno.h>`, values like `ENOENT`, `EACCES`, `EINVAL`, `EINTR`, `ENOMEM`). errno is only meaningful right after a failed call; successful calls do not clear it → save errno in a variable before calling anything else. Each thread has its own errno.
- `strerror(errno)` → a description string; `perror("msg")` prints `msg: description` to stderr.
- **EINTR**: a blocking system call (read, sleep, wait...) was interrupted by a signal → call it again.
- The book's classification: errors caused by bugs (use assert/abort), errors caused by the environment or input (report them clearly, exit with a non-zero code or recover). When failing midway, **release what was already allocated** (memory, fds) before returning.

### Libraries (2.3)
- **Archive (static library)** `.a`: a collection of `.o` files. Create: `ar cr libtest.a test1.o test2.o`. Link: `gcc -o app app.o -L. -ltest`. The linker only takes the objects from an archive that resolve **symbols still undefined at that point** → the library must come **after** the files that use it on the command line (`gcc -L. -ltest app.o` fails with undefined references).
- **Shared library** `.so`: compile with `-fPIC` (position-independent code: runs wherever it is loaded), create with `gcc -shared -fPIC -o libtest.so test1.o test2.o`. The library's code is not copied into the executable; it is loaded at run time and **shared** in memory between processes.
- With both `libtest.a` and `libtest.so` available, the linker **prefers the .so** unless you pass `-static`.
- At run time the dynamic loader looks for `.so` files in `/lib`, `/usr/lib`, the directories in `/etc/ld.so.conf` (run `ldconfig`), `LD_LIBRARY_PATH` (during development), or a path baked in with `-Wl,-rpath,/opt/lib`. `ldd app` lists the shared libraries app needs.
- Standard libraries: `libc` (linked automatically), `libm` (math, needs `-lm`), `libstdc++` (linked automatically by g++), `libpthread` (`-lpthread`).
- **Dependencies**: a shared library records which libraries it needs; an archive does not → when linking `-static` you must list them all yourself (e.g. `-ltiff -ljpeg -lz`).
- **Pros and cons**: shared libraries save disk/RAM and can be updated without relinking, but the right version must be installed on the target and a broken update can break every program using it. Static linking is self-contained but bigger; the book recommends it for setuid programs or when you cannot control the target machine.
- **Dynamic loading** (`<dlfcn.h>`, link `-ldl`): `dlopen("libtest.so", RTLD_LAZY)` → a handle, `dlsym(handle, "my_function")` → an address, `dlclose(handle)`, `dlerror()` → an error string. Used for plugins. C++ functions need `extern "C"` so dlsym can find the name (no name mangling).

## Quiz

### Which command compiles `main.c` into an object file without linking? {#alp-ch2-001}
- [ ] `gcc main.c`
- [x] `gcc -c main.c`
- [ ] `gcc -o main.c`
- [ ] `gcc -S main.c`
> `-c` stops after compilation and produces `main.o`. `-S` stops at assembly (`main.s`). With no option gcc compiles and links into `a.out`.

### A program consists of `main.c` and `reciprocal.cpp`. Which command should link it? {#alp-ch2-002}
- [ ] `gcc -o reciprocal main.o reciprocal.o`
- [x] `g++ -o reciprocal main.o reciprocal.o`
- [ ] `ld -o reciprocal main.o reciprocal.o`
- [ ] `ar cr reciprocal main.o reciprocal.o`
> With C++ code, link with g++ so the C++ standard library (libstdc++) is pulled in. gcc usually fails with undefined references to std symbols.

### What does the `-D NDEBUG` compiler option do? {#alp-ch2-003}
- [ ] Turns on debug mode
- [x] Defines the NDEBUG macro, which removes every `assert` from the program
- [ ] Turns off all warnings
- [ ] Omits debug info
> `-D` defines a macro like `#define`. assert.h checks NDEBUG to decide whether to generate the check.

### What must a command line in a Makefile rule start with? {#alp-ch2-004}
- [ ] 4 spaces
- [x] A TAB character
- [ ] A `$`
- [ ] An `@`
> Without a TAB, make reports "missing separator". An `@` at the start of the command (after the TAB) just hides the command.

### When does make rebuild a target? {#alp-ch2-005}
- [ ] On every run
- [x] When the target does not exist or a dependency is newer than the target (by modification time)
- [ ] When the source contents change (by hash)
- [ ] Only after `make clean`
> Make compares timestamps, not contents. `touch file.c` is enough to force a rebuild.

### In gdb, the program just crashed with a segfault. Which command shows the chain of calls that led there? {#alp-ch2-006}
- [ ] `print`
- [ ] `next`
- [x] `where` (or `backtrace`)
- [ ] `list`
> Then use `up`/`down` to move between frames and `print` to inspect variables there. You need `-g` for function names and line numbers.

### In gdb, how do `next` and `step` differ? {#alp-ch2-007}
- [ ] No difference
- [x] `next` runs the whole call on the current line as one step; `step` goes inside the function
- [ ] `step` runs to the next breakpoint
- [ ] `next` goes back one line
> `finish` runs until the current function returns; `continue` runs to the next breakpoint.

### To read the man page of the system call `open` (not a shell command), you type: {#alp-ch2-008}
- [ ] `man open`
- [x] `man 2 open`
- [ ] `man 3 open`
- [ ] `info open`
> Section 1: user commands, 2: system calls, 3: C library functions, 8: admin commands. E.g. `man 3 sleep` is the C function, `man sleep` the shell command.

### For `prog -o out.txt in1`, what are `argc` and `argv[0]`? {#alp-ch2-009}
- [ ] 3 and "-o"
- [x] 4 and "prog" (the program name as invoked)
- [ ] 3 and "prog"
- [ ] 4 and "out.txt"
> argv includes the program name: {"prog", "-o", "out.txt", "in1"}, and `argv[argc]` is NULL.

### In getopt_long's short-options string `"ho:v"`, what does the `:` after `o` mean? {#alp-ch2-010}
- [ ] `-o` is mandatory
- [x] `-o` takes an argument, available in `optarg`
- [ ] `-o` can be repeated
- [ ] `-o` is a long option
> E.g. `-o out.txt` or `-oout.txt`. Two colons `::` mean an optional argument (a GNU extension).

### After the getopt_long loop (returned -1), what does `optind` tell you? {#alp-ch2-011}
- [ ] How many options were processed
- [x] The index in argv of the first non-option argument
- [ ] The index of the invalid option
- [ ] It always equals argc
> Process input files with `for (i = optind; i < argc; ++i)`. GNU getopt has permuted argv so non-options are last.

### What does getopt_long return for an option that was not declared? {#alp-ch2-012}
- [ ] -1
- [ ] 0
- [x] `'?'`
- [ ] That option's character
> The offending character is in `optopt`; getopt prints a message to stderr (unless `opterr = 0`). Listing 2.2 then prints the usage to stderr and exits with code 1.

### Why does this loop not print one dot per second on a terminal? {#alp-ch2-013}
```c
while (1) { printf("."); sleep(1); }
```
- [ ] printf fails without `\n`
- [x] stdout is buffered and there is no `\n`, so the dots sit in the buffer
- [ ] sleep blocks printf too
- [ ] The terminal does not show dots
> Use `fprintf(stderr, ".")` (stderr is unbuffered) or call `fflush(stdout)` after each printf.

### Which command sends both stdout and stderr of `prog` to `out.txt`? {#alp-ch2-014}
- [ ] `prog 2>&1 > out.txt`
- [x] `prog > out.txt 2>&1`
- [ ] `prog > out.txt 1>&2`
- [ ] `prog | out.txt 2>&1`
> Redirections are processed left to right: `> out.txt` points fd 1 at the file, then `2>&1` makes fd 2 follow. The other way round, fd 2 still goes to the terminal. With a pipe: `prog 2>&1 | filter`.

### A program calls `exit(256)`. What does `echo $?` print? {#alp-ch2-015}
- [ ] 256
- [x] 0
- [ ] 1
- [ ] 255
> Only the low 8 bits survive: 256 mod 256 = 0, which the shell reads as "success". The book recommends exit codes 0–127 only.

### A program is killed with `kill -TERM` (SIGTERM = 15). What does `echo $?` print in bash? {#alp-ch2-016}
- [ ] 15
- [ ] 0
- [x] 143
- [ ] 255
> When killed by a signal, the shell reports 128 + the signal number. Ctrl+C (SIGINT = 2) gives 130.

### A parent calls `setenv("MODE", "debug", 1)` and then forks. Which are TRUE? (select all that apply) {#alp-ch2-017}
- [x] The child sees MODE=debug
- [x] A program the child execs (execvp) also sees MODE=debug
- [ ] The shell that started the parent also sees MODE=debug
- [ ] If the child calls unsetenv("MODE"), the parent loses it too
> The environment is copied from parent to child at fork and kept across exec. No process can change the environment of its parent or of another process.

### What does `getenv("SERVER_NAME")` return when the variable does not exist? {#alp-ch2-018}
- [ ] An empty string ""
- [x] NULL
- [ ] It fails with errno = ENOENT
- [ ] "SERVER_NAME"
> Listing 2.4 checks for NULL and falls back to a default.

### Why does the book recommend `mkstemp` over `tmpnam` + `open`? {#alp-ch2-019}
- [ ] mkstemp is faster
- [x] tmpnam only generates a name; between generating and opening it an attacker can create that file first (a race condition). mkstemp creates and opens the file in one step with mode 0600
- [ ] tmpnam was removed from glibc
- [ ] mkstemp deletes the file automatically when the program exits
> mkstemp does not delete the file; you must unlink it. tmpfile returns a file that is already unlinked.

### `unlink` is called on a temporary file right after mkstemp opened it. What happens? {#alp-ch2-020}
- [ ] The fd can no longer be used
- [x] The name disappears but the data can still be read/written through the fd; the file is really deleted when the fd is closed (even if the program crashes)
- [ ] unlink fails with EBUSY because the file is open
- [ ] The file is deleted immediately and the fd points to garbage
> Files are reference-counted: they are only destroyed when no name (link) and no open fd remain.

### What is wrong with this line? {#alp-ch2-021}
```c
assert(do_something() == 0);
```
- [ ] assert does not accept comparisons
- [x] When compiled with -DNDEBUG, the call to do_something() is removed too
- [ ] assert only works in C++
- [ ] Nothing
> Write `int status = do_something(); assert(status == 0);`. assert expressions must have no side effects.

### When should you use assert? {#alp-ch2-022}
- [ ] To check that the user typed a valid number
- [ ] To check that a configuration file exists
- [x] To check that a pointer parameter is not NULL, as required by the function's contract
- [ ] To check that a network connection succeeded
> assert is for programming errors (things that "can never happen"). Input/environment errors must be handled and reported properly, because assert may be disabled and crashes the program.

### After a failed `open()` (errno = ENOENT), the program calls `getpid()` (which succeeds). What is errno now? {#alp-ch2-023}
- [ ] 0
- [x] Still ENOENT
- [ ] Undefined
- [ ] EINVAL
> Successful calls do not clear errno. Read errno right after a failure, and save it if you call anything else (even printf).

### A blocking system call (read, wait) returns -1 with errno = EINTR. What does it mean and what should you do? {#alp-ch2-024}
- [ ] The file is corrupt, exit
- [x] A signal interrupted the call; usually just call it again
- [ ] Permission denied
- [ ] Invalid argument
> Pattern: `do { n = read(fd, buf, len); } while (n == -1 && errno == EINTR);`. The `SA_RESTART` flag of sigaction makes many system calls restart automatically.

### Is errno safe to use in a multithreaded program? {#alp-ch2-025}
- [ ] No, all threads share one errno
- [x] Yes, each thread has its own errno
- [ ] Only with a mutex
- [ ] Only in the main thread
> In glibc, errno is a macro referring to a thread-local variable. In contrast, pthread functions return error codes directly and do not set errno.

### Which command creates the static library `libtest.a` from `test1.o` and `test2.o`? {#alp-ch2-026}
- [ ] `gcc -shared -o libtest.a test1.o test2.o`
- [x] `ar cr libtest.a test1.o test2.o`
- [ ] `ld -o libtest.a test1.o test2.o`
- [ ] `gcc -static -o libtest.a test1.o test2.o`
> `c`: create the archive if needed, `r`: insert or replace objects. Shared libraries are created with `gcc -shared -fPIC`.

### `gcc -o app -L. -ltest app.o` reports undefined references to functions in libtest.a, even though they are there. Why? {#alp-ch2-027}
- [ ] `-fPIC` is missing
- [x] The linker works left to right; when it reads libtest.a no symbols are needed yet, so it takes no objects. `-ltest` must come after `app.o`
- [ ] Archives do not work with -L
- [ ] You must use `-static`
> From an archive the linker only takes objects that resolve currently missing symbols. Mutually dependent libraries may have to be listed more than once.

### What is `-fPIC` for when building a shared library? {#alp-ch2-028}
- [ ] Optimization
- [x] Generating position-independent code, so the library runs at any load address and can be shared between processes
- [ ] Adding debug info
- [ ] Static linking
> PIC = Position-Independent Code: global data/functions are accessed through tables (GOT/PLT) instead of absolute addresses.

### A directory contains both `libtest.a` and `libtest.so`. Which does `gcc -o app app.o -L. -ltest` use? {#alp-ch2-029}
- [ ] libtest.a
- [x] libtest.so, unless `-static` is given
- [ ] It reports an ambiguity
- [ ] Whichever is newer
> The linker prefers the shared library. `-static` forces the archive (and links libc statically too).

### A program linked against `libtest.so` in `/home/me/lib` fails at run time with "cannot open shared object file". Which fixes work? (select all that apply) {#alp-ch2-030}
- [x] `export LD_LIBRARY_PATH=/home/me/lib` and run it again
- [x] Relink with `-Wl,-rpath,/home/me/lib`
- [x] Add the directory to `/etc/ld.so.conf` and run `ldconfig` (needs root)
- [ ] Add `-I/home/me/lib` when compiling
> `-L` only helps at link time; at run time the dynamic loader has its own search list. `-I` is a header directory. `ldd app` shows which libraries are "not found".

### `sqrt` compiles but linking fails with `undefined reference to 'sqrt'`. What is missing? {#alp-ch2-031}
- [ ] `#include <math.h>`
- [x] `-lm` when linking
- [ ] `-fPIC`
- [ ] `-DMATH`
> Math functions live in libm, which is not linked automatically like libc. A missing `#include` is a compile-time error/warning, not a link error.

### Advantages of shared libraries over static ones? (select all that apply) {#alp-ch2-032}
- [x] Smaller executables
- [x] Many processes share one copy in memory
- [x] Fixing a bug in the library does not require relinking programs
- [ ] The program runs on machines without the library installed
> Running stand-alone is an advantage of static linking. Shared-library risks: missing or wrong versions on the target, and one bad update breaking every program that uses it.

### Which function gets the address of the symbol `my_function` after `dlopen`? {#alp-ch2-033}
- [ ] `dlopen`
- [x] `dlsym`
- [ ] `dlerror`
- [ ] `dladdr`
> `void *h = dlopen("libtest.so", RTLD_LAZY); void (*f)() = dlsym(h, "my_function"); (*f)(); dlclose(h);`. Read errors with `dlerror()`.

### Why must a C++ function you want to look up with dlsym be declared `extern "C"`? {#alp-ch2-034}
- [ ] For speed
- [x] So the symbol name is not mangled and dlsym can find the string "my_function"
- [ ] Because dlsym only works with C
- [ ] To make the function static
> g++ encodes the parameter types into the name (e.g. `_Z11my_functionv`). extern "C" keeps the plain C name.
