# Build, link and debug

## Theory

### Compilation stages
| Stage | Command | Output |
| --- | --- | --- |
| Preprocess | `gcc -E a.c` | .i (#include and #define expanded) |
| Compile | `gcc -S a.c` | .s (assembly) |
| Assemble | `gcc -c a.c` | .o (object file, unlinked machine code) |
| Link | `gcc a.o b.o -o app` | ELF executable |

- Syntax/type errors → the compile stage. `undefined reference to 'f'` → the **link** stage (declared, but the definition was not linked in). `multiple definition` → defined in two files.
- Common flags: `-Wall -Wextra` (warnings), `-g` (debug info), `-O0/-O2/-O3/-Os` (optimization), `-I dir` (header directory), `-L dir -lname` (library directory and libname), `-D NAME=val`, `-std=c11`, `-static`, `-fPIC`, `-fsanitize=address,undefined`.

### Libraries
- **Static** `.a` (an archive of .o files, made with `ar rcs libx.a a.o b.o`): copied into the executable at link time → bigger file, no run-time dependency, updating the library means relinking.
- **Shared** `.so` (`gcc -shared -fPIC -o libx.so a.c`): loaded at run time by the dynamic linker (`ld-linux.so`); many programs share one copy in RAM, updates need no rebuild. Found via `LD_LIBRARY_PATH`, `/etc/ld.so.conf` (`ldconfig`), rpath. Show dependencies: `ldd app`.
- The order of `-l` matters for static libraries: put the library **after** the files that use it.
- `extern "C"` when calling C functions from C++ (turns off name mangling).

### ELF and binutils
- Sections: `.text` (code), `.rodata`, `.data`, `.bss`, `.symtab`, `.debug_*`.
- `file` (file type), `size` (text/data/bss sizes), `nm` (symbols: T text, D data, B bss, U undefined), `objdump -d` (disassemble), `readelf -h/-S/-s`, `strings`, `strip` (remove symbols), `addr2line`.

### Make
```make
CC = gcc
CFLAGS = -Wall -g
app: main.o util.o
	$(CC) $^ -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY: clean
clean:
	rm -f *.o app
```
- Rule: `target: prerequisites` + commands (starting with a **TAB**). Rebuilt only when a prerequisite is newer than the target.
- `$@` target, `$<` first prerequisite, `$^` all prerequisites. `.PHONY` for targets that are not files. `=` deferred assignment, `:=` immediate. `make -j4` builds in parallel.
- CMake generates Makefiles/Ninja: `cmake -B build && cmake --build build`.

### Debugging
- **gdb**: `gdb ./app` → `break main`/`b file.c:20`, `run args`, `next` (n, steps over calls), `step` (s, steps into calls), `continue` (c), `finish`, `print x`, `display x`, `watch x` (stop when x changes), `backtrace` (bt), `frame n`, `info locals`, `x/4xw addr` (examine memory), `set var x = 5`, `quit`. Core dumps: `ulimit -c unlimited`, then `gdb ./app core`. Attach to a running process: `gdb -p PID`. Remote (embedded): `gdbserver` + `target remote`.
- **valgrind** (memcheck): memory leaks, out-of-bounds reads/writes, use after free, uninitialized variables. `valgrind --leak-check=full ./app`.
- **AddressSanitizer** `-fsanitize=address`: much faster than valgrind, but needs a rebuild.
- `strace` (system calls), `ltrace` (library calls), `perf` (performance), `objdump`.
- Cross-compiling for embedded: toolchains like `arm-linux-gnueabihf-gcc`, `aarch64-linux-gnu-gcc`; build on the host (x86), run on the target (ARM).

### Git basics
`clone`, `status`, `add`, `commit`, `log --oneline`, `diff`, `branch`, `checkout`/`switch`, `merge`, `rebase`, `pull` (= fetch + merge), `push`, `stash`, `reset --soft/--mixed/--hard`, `revert` (creates an inverse commit, safe for pushed history), `cherry-pick`, `tag`.

## Quiz

### At which stage does `undefined reference to 'foo'` happen?
- [ ] Preprocess
- [ ] Compile
- [ ] Assemble
- [x] Link
> The compiler accepts it because there is a declaration, but the linker cannot find the definition (a forgotten .c file or -llibrary).

### Which gcc option only compiles to an object file (.o) without linking?
- [ ] -E
- [ ] -S
- [x] -c
- [ ] -o
> -E stops after preprocessing, -S after compiling (gives .s), -o names the output.

### Advantages of shared libraries (.so) over static libraries (.a)? (select all that apply)
- [x] Smaller executables
- [x] Many processes share one copy in RAM
- [x] Updating the library does not require relinking programs
- [ ] The library does not need to be present at run time
> Drawbacks: the right .so version must be available at run time, and start-up is slightly slower.

### Which command lists the shared libraries a program depends on?
- [ ] nm app
- [x] ldd app
- [ ] objdump -h app
- [ ] file app
> nm lists symbols, file tells you the file type (ELF, dynamically linked...).

### In `nm` output, what does `U` in front of a symbol mean?
- [ ] Unused
- [x] Undefined — used here but defined elsewhere
- [ ] Unsigned
- [ ] User-defined
> T: function in .text, D: variable in .data, B: variable in .bss, R: rodata. Lowercase = local (static) symbol.

### In a Makefile, what are `$@` and `$<`?
- [x] The target name and the first prerequisite
- [ ] All prerequisites and the target name
- [ ] The current directory and the file name
- [ ] The compiler name and the compiler flags
> $^ is all prerequisites.

### What must a command line in a Makefile rule start with?
- [ ] 4 spaces
- [x] A TAB character
- [ ] An @
- [ ] A -
> Spaces give "missing separator". @ (hide the command) and - (ignore errors) are optional prefixes after the TAB.

### Why do you need `.PHONY: clean`?
- [ ] To make clean faster
- [x] So make always runs the clean rule even if a file named "clean" exists
- [ ] To run clean before every rule
- [ ] So make does not print the commands
> Otherwise, if a file named clean exists, make considers the target up to date and skips it.

### In gdb, what is the difference between `next` and `step`?
- [ ] None
- [x] next runs over function calls, step goes into the function
- [ ] step runs to the next breakpoint
- [ ] next goes back one line
> finish runs until the current function returns; continue runs to the next breakpoint.

### Which gdb command stops the program when the variable `count` changes?
- [ ] break count
- [x] watch count
- [ ] display count
- [ ] print count
> A watchpoint (in hardware when possible). display prints the value at every stop.

### Which gdb command shows the call stack after a crash?
- [ ] stack
- [x] backtrace (bt)
- [ ] info frame all
- [ ] where-is
> `frame n` switches to frame n, `info locals` shows local variables. `where` is another name for backtrace.

### Which tool detects memory leaks WITHOUT rebuilding the program?
- [x] valgrind
- [ ] -fsanitize=address
- [ ] gcc -Wall
- [ ] gdb watch
> ASan needs a rebuild with -fsanitize=address but runs much faster than valgrind.

### What does cross-compilation mean?
- [ ] Compiling C and C++ at the same time
- [x] Compiling on one architecture (the host, e.g. x86) to run on another (the target, e.g. ARM)
- [ ] Compiling with several compilers
- [ ] Compiling many files in parallel
> E.g. `arm-linux-gnueabihf-gcc main.c -o main` on a PC, then copy it to the board.

### What is `extern "C"` for in C++ code?
- [ ] Compiling that part with a C compiler
- [x] Turning off name mangling so it links against functions written in C
- [ ] Declaring a global variable
- [ ] Allowing malloc
> C headers are usually wrapped in `#ifdef __cplusplus extern "C" { #endif`.

### Which git command undoes a pushed commit without rewriting history?
- [ ] git reset --hard HEAD~1
- [x] git revert <commit>
- [ ] git checkout HEAD~1
- [ ] git commit --amend
> revert creates a new commit that reverses the changes. reset/amend rewrite history and need a force push.

### `git pull` is equivalent to?
- [ ] git clone + git merge
- [x] git fetch + git merge (or rebase, if configured)
- [ ] git push + git fetch
- [ ] git checkout + git reset
> `git pull --rebase` rebases instead of merging.

### A file-scope `static int x;` in a.c and `int x;` in b.c are linked together. What happens?
- [ ] multiple definition error
- [x] Two different variables, no conflict
- [ ] Both names refer to the same variable
- [ ] undefined reference error
> static gives internal linkage, so the x in a.c is not visible outside it.
