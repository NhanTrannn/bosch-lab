# bosch-lab — hands-on C/C++ & Linux System labs

Preparation for the **C++ and Linux System Knowledge** assessment (Thursday 29 Oct 2026). Each lab is a short, self-contained program.
**Predict the output before running it**, then compare with `ANSWERS.md` (or the lab page in the web UI).

## Setup (Ubuntu)
```sh
sudo apt install build-essential gdb strace ltrace valgrind
```
VS Code: install the **C/C++** extension (Microsoft).

## Build and run
```sh
make                                  # build everything into build/
make run T=03-process/01_fork_count   # build and run one lab
make clean
```
- Flags: `-Wall -Wextra -g`, C uses `-std=c11`, C++ uses `-std=c++17`, thread labs add `-pthread`.
- **No `-O2` on purpose**: the undefined-behavior / volatile / race-condition labs give results that are easier to observe and closer to the answers. Turning on `-O2` can change the results (which is a lesson in itself).
- Some labs have **intentional compiler warnings** (listed in the header comment, on the "Intentional compiler warning" line).
- POSIX code starts with `#define _GNU_SOURCE` because strict `-std=c11` hides functions such as `sigaction` and `kill`.
- VS Code: open a lab file → **Ctrl+Shift+B** (task "Build & run current file"); **F5** to debug with gdb (set a breakpoint first).

## Web UI (labs + theory + quizzes)
```sh
make web        # then open http://localhost:8000
npm start       # the same, if Node/npm is installed
```
Other npm commands: `npm run build` (= make), `npm run clean`, `npm run lab 03-process/01_fork_count` (= make run T=...).
- **Labs**: pick a topic (labs are grouped like the theory and quiz pages), then in each lab predict the output → build & run it right in the browser (a real terminal: Ctrl+C/Ctrl+Z/stdin/kill/ps) → compare your prediction with the real output → read the answer and self-grade.
- **Theory**: summaries in 19 topics (including a "Tricky questions" trap list), read from `09-theory/*.md`.
- **Quiz**: 529 questions with explanations; click a topic to answer all of its questions, mix all topics, or review the ones you got wrong. A 20-question mock exam is on the Overview page.
- **Terminal 2** (bottom right): a real bash for labs that need 2 terminals.
- Only needs the Python 3 that ships with Ubuntu; nothing else to install. Progress is stored in your browser.
- Frontend/backend layout and API: see `web/README.md`.

## Labs to note
- Need **2 terminals**: `03-process/05_zombie`, `04-signal/03_kill_test`, `06-ipc/03_fifo`.
- **Hang / run long on purpose** (press **Ctrl+C** to quit): `03-process/05_zombie`, `04-signal/01_sigint_handler`, `04-signal/03_kill_test`, `05-thread/03_deadlock`.
- `07-kernel-module` is **not** part of the top-level `make` — see `07-kernel-module/README.md`.
- `08-shell/drills.md`: 20 command drills, nothing to compile.
- `04-signal/04_alarm_pause` dies on purpose from SIGALRM at the end ("Alarm clock").
- `10-alp/05_errno_assert` dies on purpose from a failed assert at the end (SIGABRT, "Aborted").
- Personal notes (e.g. copied from a Udemy course): put them in `09-theory/private/*.md` in the same format; that directory is in `.gitignore` and only shows up on your machine.
- Adding quiz questions: write them in the `## Quiz` section of a file in `09-theory/` using `### question` / `- [ ]` wrong / `- [x]` right / `> explanation`.

## Suggested schedule
| Day | Directories |
| --- | --- |
| Monday 26 Oct | `01-c`, `02-cpp` |
| Tuesday 27 Oct | `03-process`, `04-signal`, `08-shell` |
| Wednesday 28 Oct | `05-thread`, `06-ipc`, `07-kernel-module`, `10-alp` |
| Thursday 29 Oct (morning) | Only reread `ANSWERS.md` and redo the labs/questions you got wrong |

## Labs
### 01-c
- `01_sizeof_padding.c` — sizeof and struct padding
- `02_pointer_array.c` — Pointer arithmetic, arrays and decay
- `03_const_pointer.c` — The three kinds of const with pointers
- `04_static_extern.c + 04_helper.c` — static locals, file-scope static and extern
- `05_bitwise.c` — Bit manipulation
- `06_endian_union.c` — Endianness with a pointer and a union
- `07_signed_unsigned.c` — signed/unsigned traps and integer division
- `08_macro_traps.c` — Macro traps
- `09_memory_layout.c` — Memory layout: text, data, bss, heap, stack
- `10_switch_fallthrough.c` — switch without break
- `11_string_array_pointer.c` — Strings: char arrays and pointers to string literals
- `12_precedence.c` — Operator precedence and short-circuit evaluation
- `13_function_pointer.c` — Function pointers, arrays of function pointers and callbacks (qsort)
- `14_integer_promotion.c` — Integer promotion: char/short automatically become int
- `15_float.c` — Floating point: comparison, rounding, integer division
- `16_scope_storage.c` — Variable scope, shadowing and static inside a loop

### 02-cpp
- `01_ctor_dtor_order.cpp` — Constructor / destructor order
- `02_virtual_slicing.cpp` — virtual, calls through an object/pointer/reference, and object slicing
- `03_virtual_dtor.cpp` — Virtual and non-virtual destructors
- `04_virtual_in_ctor.cpp` — Calling virtual functions in constructors/destructors
- `05_rule_of_five.cpp` — Rule of five: copy/move constructors and assignment
- `06_smart_ptr.cpp` — unique_ptr, shared_ptr, weak_ptr
- `07_lambda_capture.cpp` — Lambda capture by value and by reference
- `08_stl_traps.cpp` — STL traps: map[], erasing in a loop, iterator invalidation
- `09_name_hiding.cpp` — Name hiding and using Base::f
- `10_virtual_default_arg.cpp` — Default arguments of virtual functions, and virtual calls through a normal function
- `11_exception_raii.cpp` — Exceptions, stack unwinding and RAII
- `12_diamond.cpp` — Multiple inheritance, the diamond and virtual inheritance
- `13_static_member.cpp` — static members, static locals and the singleton
- `14_move_noexcept.cpp` — std::move, const and noexcept when a vector reallocates
- `15_sizeof_class.cpp` — sizeof of classes: empty classes, the vptr, padding, static members
- `16_templates.cpp` — Templates: type deduction, specialization, overloading with a normal function

Following the Udemy course *Beginning C++ Programming* (sections 6–23):
- `17_types_conversion.cpp` — Variables, fundamental types, integer division and conversions
- `18_operators.cpp` — Operators: ++/--, compound assignment, comparison, logical operators and short-circuit
- `19_control_flow.cpp` — Control flow: range-for copy/reference, do-while, continue/break, dangling else, the ?: operator
- `20_strings.cpp` — C-style strings and std::string
- `21_functions.cpp` — Functions: pass by value/reference/pointer, default arguments, overloading, static locals, recursion
- `22_pointers_refs.cpp` — Pointers and references: reseating, assigning through a reference, pointer arithmetic, passing pointers to functions
- `23_class_basics.cpp` — Classes: constructor overloading, delegating constructors, the copy constructor, const methods, static members, shallow/deep copy
- `24_operator_overloading.cpp` — Operator overloading: + - == ++ (prefix/postfix) += [] << and non-member functions
- `25_inheritance.cpp` — Inheritance and polymorphism: calling the base constructor, protected, redefined (static binding) vs virtual (dynamic binding) functions
- `26_exceptions.cpp` — Exceptions: throwing any type, catch order, catch(...), rethrow, custom exception classes
- `27_streams.cpp` — I/O streams: formatting with iomanip, sticky and non-sticky manipulators, stringstream, error state
- `28_stl_algorithms.cpp` — STL: iterators, algorithms, accumulate, remove does not erase, map/set, deque, stack/queue/priority_queue
- `29_lambda_enum.cpp` — Lambdas (captures, mutable, use with algorithms) and enum / enum class

### 03-process
- `01_fork_count.c` — fork() three times
- `02_fork_if.c` — fork() inside if and inside logical expressions
- `03_fork_buffer.c` — stdio buffers and fork
- `04_fork_exec_wait.c` — fork + exec + waitpid
- `05_zombie.c` — Zombie processes
- `06_orphan.c` — Orphan processes
- `07_fork_cow.c` — Variables after fork(): copy-on-write
- `08_fork_fd_offset.c` — File descriptors after fork(): a shared offset
- `09_exec_buffer.c` — exec() replaces the program: the PID stays, the stdio buffer is lost

### 04-signal
- `01_sigint_handler.c` — Catching Ctrl+C with sigaction
- `02_sigchld_reap.c` — Reaping zombies with SIGCHLD
- `03_kill_test.c` — Sending signals with the kill command
- `04_alarm_pause.c` — alarm, pause and the default action of SIGALRM
- `05_sigprocmask.c` — Blocking signals (sigprocmask), pending signals, and why signals are not queued

### 05-thread
- `01_race_condition.c` — A race condition with counter++
- `02_mutex_fix.c` — Fixing the race with pthread_mutex
- `03_deadlock.c` — Deadlock from locking in opposite orders
- `04_cond_var.c` — A one-slot producer–consumer with a condition variable
- `05_semaphore.c` — A semaphore that limits threads to 2 at a time
- `06_cpp_thread.cpp` — std::thread, std::mutex, lock_guard, std::atomic
- `07_thread_arg_bug.c` — Passing &i to threads, and getting results back through pthread_join
- `08_thread_local.c` — What threads share and what they own: globals, thread-locals, PID/TID

### 06-ipc
- `01_pipe.c` — pipe: the parent writes, the child reads
- `02_pipe_dup2.c` — Implementing ls / | wc -l with dup2
- `03_fifo_reader.c` — A named pipe (FIFO) between 2 programs
- `03_fifo_writer.c` — Named pipe — the writing side (goes with 03_fifo_reader.c)
- `04_shm.c` — POSIX shared memory between a parent and a child
- `05_mmap_file.c` — Mapping a file into memory with mmap
- `06_unix_socket.c` — UNIX domain socket: a server/client pair using fork
- `07_mqueue.c` — POSIX message queues and priorities
- `08_sigpipe.c` — Writing to a pipe with no reader: SIGPIPE and EPIPE

### 10-alp (the book *Advanced Linux Programming*, chapters 1–5)
- `01_getopt_long.c` — getopt_long: short/long options, optarg, optind, "--" and argv permutation (ALP 2.1.3)
- `02_env_exit_code.c` — Environment variables (getenv/setenv/environ) and exit codes, including death by signal (ALP 2.1.5, 2.1.6, 3.4)
- `03_stdio_buffer.c` — stdio buffering: full, line, unbuffered; stderr is unbuffered; exit vs _exit (ALP 2.1.4)
- `04_tempfile.c` — Safe temporary files: mkstemp + immediate unlink, tmpfile (ALP 2.1.7, listing 2.5)
- `05_errno_assert.c` — errno, strerror, perror and assert (including NDEBUG) (ALP 2.2)
- `06_dlopen.c` — Loading a shared library at run time: dlopen, dlsym, dlerror, dlclose (ALP 2.3.6)
- `07_fork_exec.c` — system, fork + exec (the l/v/p/e exec family), spawn as in listing 3.4, nice (ALP 3.2)
- `08_wait_status.c` — wait/waitpid and the status macros, WNOHANG, zombies in /proc (ALP 3.4)
- `09_thread_return.c` — pthreads: passing data to a thread, return values through join, pthread_exit, detached threads (ALP 4.1)
- `10_thread_cancel.c` — Thread cancellation: deferred cancellation, cancellation points, uncancelable sections, cleanup handlers (ALP 4.2, 4.3.1)
- `11_tsd_mutex.c` — Thread-specific data (pthread_key) and kinds of mutex: fast, error-checking, recursive, trylock, semaphores (ALP 4.3, 4.4)
- `12_sysv_shm_sem.c` — System V shared memory and semaphores: shmget/shmat/shmctl, semget/semop, SEM_UNDO (ALP 5.1, 5.2)
- `13_mmap_shared_private.c` — mmap: MAP_SHARED and MAP_PRIVATE on a file, msync, anonymous mappings shared through fork (ALP 5.3)
- `14_popen_fdopen.c` — Pipes with FILE*: fdopen (listing 5.7), dup2 + exec sort (listing 5.8), popen/pclose (listing 5.9)
- `15_sockets.c` — Sockets: socketpair, an Internet-domain socket over 127.0.0.1 (server: socket/bind/listen/accept, client: connect), byte order (ALP 5.5)

### 07-kernel-module
- `hello.c + Makefile + README.md` — Hello kernel module

### 08-shell
- `drills.md` — 20 shell command drills

### 11-design-patterns (following the tutorialspoint Design Patterns tutorial)
- `01_singleton.cpp` — Singleton: lazy Meyers singleton, deleted copy, thread-safe initialization
- `02_factory.cpp` — Factory, Factory Method and Abstract Factory
- `03_builder_prototype.cpp` — Builder and Prototype
- `04_adapter_bridge.cpp` — Adapter and Bridge
- `05_composite_decorator.cpp` — Composite and Decorator
- `06_facade_proxy_flyweight.cpp` — Facade, Proxy and Flyweight
- `07_observer.cpp` — Observer (publish/subscribe), with detaching and weak_ptr observers
- `08_strategy_state_template.cpp` — Strategy, State and Template Method
- `09_command_memento_chain.cpp` — Command (with undo), Memento and Chain of Responsibility
- `10_iterator_visitor_mediator.cpp` — Iterator, Visitor and Mediator
- `11_interpreter_filter_null.cpp` — Interpreter, Filter (Criteria) and Null Object
- `12_j2ee_patterns.cpp` — Enterprise (J2EE) patterns in C++: MVC, DAO + Transfer Object, Business Delegate + Service Locator

### 09-theory (theory + quizzes)
- `01-c.md` · `02-cpp-oop.md` · `03-cpp-stl-modern.md` · `04-process-memory.md` · `05-signal-ipc.md` · `06-thread-sync.md` · `07-linux-shell-fs.md` · `08-build-debug.md` · `09-kernel-embedded.md`
- Following the Udemy course: `10-cpp-basics.md` · `11-cpp-functions-pointers.md` · `12-cpp-classes-operators.md` · `13-cpp-exceptions-io-stl.md`
- Following the book Advanced Linux Programming: `14-alp-ch1-2-gnu-tools.md` · `15-alp-ch3-processes.md` · `16-alp-ch4-threads.md` · `17-alp-ch5-ipc.md`
- Design patterns: `21-design-patterns.md`
