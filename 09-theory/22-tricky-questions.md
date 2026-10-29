# Tricky questions (C, C++, Linux)

## Theory

Short questions that look easy but hide a classic trap. Every output below was checked with gcc/g++ on x86-64 Linux. Read the trap list first, then do the quiz.

### C traps
| Trap | What really happens |
| --- | --- |
| `sizeof` on an array parameter | `void f(int a[10])` — `a` is an `int *`, so `sizeof a` is 8 (pointer size), not 40 |
| `char s[] = "abc"` | `sizeof s` = 4 (includes `'\0'`), `strlen(s)` = 3 |
| signed vs unsigned compare | `-1 < 1u` is **false**: `-1` is converted to `UINT_MAX` |
| integer division | `7/2` = 3, `-7/2` = -3 (truncates toward zero), `-7%2` = -1 (sign of the dividend) |
| `if (x = 0)` | assignment, value 0 → always false |
| macros without parentheses | `#define SQ(x) x*x` → `SQ(1+2)` = `1+2*1+2` = 5 |
| `sizeof(i++)` | the operand is not evaluated → `i` is unchanged |
| `char c = 255` | `char` is signed on x86 → `c` = -1 |
| `a[i] == i[a]` | both are `*(a + i)` → `2[a]` is legal |
| `switch` without `break` | falls through into the next cases |
| `float f = 0.1; f == 0.1` | false: `f` is rounded to float precision, `0.1` is a double |
| leading zero | `010` is octal = 8; `08` does not compile |
| `sizeof('a')` in C | 4 — a character constant has type `int` in C (it is 1 in C++) |
| `5 > 3 > 1` | `(5 > 3) > 1` = `1 > 1` = 0 |
| `i++ + ++i`, `i = i++` in C | undefined behavior (unsequenced modifications) |
| modifying a string literal | `char *p = "hi"; p[0] = 'H';` → undefined behavior (usually a segfault) |
| `for (unsigned i = n; i >= 0; i--)` | infinite loop: an unsigned value is always ≥ 0 |

### C++ traps
| Trap | What really happens |
| --- | --- |
| `vector<int> v(3, 5)` vs `v{3, 5}` | 3 elements {5,5,5} vs 2 elements {3,5} |
| virtual call in a constructor/destructor | calls the version of the class being constructed (the base), not the derived override |
| default arguments + virtual | the default comes from the **static** type, the body from the dynamic type |
| object slicing | `Base b = Derived();` copies only the base part; `b.f()` calls `Base::f` |
| `map[key]` | inserts a value-initialized element if the key is missing |
| `auto a = ref;` | `auto` drops references and top-level `const` → `a` is a copy |
| lambda `[x]` | captures the value at the moment the lambda is created |
| `auto x = {1};` | `x` is a `std::initializer_list<int>`, not an `int` |
| `cond ? 1 : 2.0` | the type is `double` (common type of both branches) |
| `'a' + 1` | `int` 98, not `'b'` |
| empty class | `sizeof` is 1, so distinct objects have distinct addresses |
| member initialization order | members are initialized in **declaration order**, not in the order of the initializer list |
| `delete` vs `delete[]` | mismatching them is undefined behavior |
| deleting through a base pointer without a virtual destructor | undefined behavior (the derived destructor is usually not run) |
| `std::move` | only a cast to an rvalue; the move happens in the move constructor/assignment |
| `class D : B` | default inheritance for `class` is **private** (public for `struct`) |
| `catch (Base&)` before `catch (Derived&)` | the derived handler is never reached |
| `v.size() - 1` on an empty vector | `size_t` wraps to a huge number |
| `"a" + "b"` | does not compile: two `const char *` cannot be added |

### Linux traps
| Trap | What really happens |
| --- | --- |
| `fork(); fork();` | 4 processes; n forks in a row (or in a loop) → 2ⁿ processes |
| `fork() && fork();` | 3 processes: the child of the first fork gets 0 and skips the second fork |
| `printf("hi"); fork();` | "hi" is still in the stdio buffer, which is copied → printed twice |
| `printf("hi\n"); fork();` | once on a terminal (line buffered), **twice** when redirected to a file or pipe (fully buffered) |
| variables after `fork()` | the child has its own copy; its changes are not visible in the parent |
| `exit(256)` | only the low 8 bits are kept → exit status 0 |
| `close(1); open(...)` | `open` returns the lowest free descriptor → 1, so stdout now goes to that file |
| `wait()` with no children | returns -1, `errno = ECHILD` |
| `exec*()` | returns only on failure |
| SIGKILL / SIGSTOP | cannot be caught, blocked or ignored |
| `kill pid` | sends SIGTERM (15) by default |
| `false \| true; echo $?` | 0: the status of a pipeline is the last command's (unless `set -o pipefail`) |
| `cmd 2>&1 >file` | stderr still goes to the terminal; redirections are applied left to right |
| `echo a \| read x` | `read` runs in a subshell, so `x` is unchanged afterwards |
| `b=$a+1` | string concatenation ("5+1"); use `$((a + 1))` |
| `grep -c` | counts matching **lines**, not matches |
| a new empty directory | link count 2 (its name in the parent + its own `.`) |
| `rm` on an open file | the name disappears; the space is freed only when the last descriptor is closed |
| `cd` inside a script | changes only the script's process, not the calling shell |

## Quiz

### What does this print on x86-64 Linux?
```c
void f(int a[10]) { printf("%zu\n", sizeof(a)); }
int main(void) { int a[10]; f(a); }
```
- [ ] 40
- [ ] 10
- [x] 8
- [ ] 4
> An array parameter is adjusted to a pointer: `int a[10]` in a parameter list means `int *a`, and a pointer is 8 bytes on x86-64.

### What does this print?
```c
char s[] = "abc";
printf("%zu %zu\n", sizeof s, strlen(s));
```
- [ ] 3 3
- [x] 4 3
- [ ] 8 3
- [ ] 4 4
> `sizeof` counts the terminating `'\0'`; `strlen` does not.

### What does this print?
```c
unsigned u = 1; int i = -1;
puts(i < u ? "less" : "not less");
```
- [ ] less
- [x] not less
- [ ] It does not compile
- [ ] Undefined behavior
> In a comparison between `int` and `unsigned int`, the `int` is converted to unsigned: -1 becomes 4294967295, which is greater than 1. Compilers warn about this with `-Wsign-compare`.

### What does `printf("%d %d %d", 7/2, -7/2, -7%2)` print?
- [ ] 3 -4 1
- [x] 3 -3 -1
- [ ] 4 -4 -1
- [ ] 3 -3 1
> Since C99, integer division truncates toward zero, and the remainder has the sign of the dividend (`a == (a/b)*b + a%b`).

### What does this print?
```c
int x = 0;
if (x = 0) puts("A"); else puts("B");
```
- [ ] A
- [x] B
- [ ] It does not compile
- [ ] A and B
> `x = 0` is an assignment whose value is 0 (false). `-Wall` warns: "suggest parentheses around assignment used as truth value".

### What does `SQ(1+2)` evaluate to?
```c
#define SQ(x) x*x
```
- [ ] 9
- [x] 5
- [ ] 6
- [ ] It does not compile
> The macro expands textually to `1+2*1+2` = 5. Write `#define SQ(x) ((x)*(x))` — and even then `SQ(i++)` increments twice.

### What does this print?
```c
int k = 1;
size_t z = sizeof(k++);
printf("%d\n", k);
```
- [x] 1
- [ ] 2
- [ ] 4
- [ ] Undefined behavior
> The operand of `sizeof` is not evaluated (except for variable-length arrays); only its type matters, so `k++` never runs.

### What does this print with gcc on x86-64 Linux?
```c
char c = 255;
printf("%d\n", c);
```
- [ ] 255
- [x] -1
- [ ] 0
- [ ] It does not compile
> Whether plain `char` is signed is implementation-defined; on x86 it is signed, so 255 does not fit and becomes -1. On ARM Linux `char` is unsigned and this prints 255.

### Does `2[a]` compile, and what is it for `int a[3] = {1, 2, 3};`?
- [ ] It does not compile
- [ ] It compiles and equals 2
- [x] It compiles and equals 3
- [ ] It compiles but is undefined behavior
> `a[i]` is defined as `*(a + i)`, and addition is commutative, so `2[a]` is `*(2 + a)` = `a[2]` = 3.

### What does this print?
```c
int cnt(void) { static int n = 0; return ++n; }
int main(void) { cnt(); cnt(); printf("%d\n", cnt()); }
```
- [ ] 1
- [ ] 0
- [x] 3
- [ ] 2
> A `static` local variable is initialized once and keeps its value between calls.

### What does this print?
```c
int y = 1;
switch (y) {
case 1: printf("A");
case 2: printf("B"); break;
case 3: printf("C");
}
```
- [ ] A
- [x] AB
- [ ] ABC
- [ ] B
> Without `break`, execution falls through from `case 1` into `case 2`, then the `break` stops it.

### What does this print?
```c
float f = 0.1;
puts(f == 0.1 ? "equal" : "not equal");
```
- [ ] equal
- [x] not equal
- [ ] It depends on the optimization level
- [ ] It does not compile
> 0.1 cannot be represented exactly. `f` holds the float approximation; `0.1` is a double with a different (more precise) approximation. Compare with a tolerance, or use `0.1f`.

### What does `printf("%d %c", '0' + 1, '0' + 1)` print?
- [ ] 1 1
- [x] 49 1
- [ ] 01 1
- [ ] 49 49
> `'0'` is 48 in ASCII; 48 + 1 = 49, which is the code of the character `'1'`.

### What is the value of `x`?
```c
int x = (1, 2, 3);
```
- [ ] 1
- [x] 3
- [ ] 6
- [ ] It does not compile
> The comma operator evaluates the left operand, discards it and yields the right one. (Without the parentheses, `int x = 1, 2, 3;` would not compile.)

### What does `printf("%d", 010 + 1)` print?
- [ ] 11
- [x] 9
- [ ] 3
- [ ] It does not compile
> A leading 0 makes an integer literal octal: `010` = 8. (`08` and `09` do not compile.)

### What is `sizeof('a')` in C, compiled with gcc on x86-64?
- [ ] 1
- [x] 4
- [ ] 8
- [ ] 2
> In C a character constant has type `int`. In C++ it has type `char`, so the same expression is 1.

### What does `printf("%d", 5 > 3 > 1)` print?
- [ ] 1
- [x] 0
- [ ] It does not compile
- [ ] Undefined behavior
> It parses as `(5 > 3) > 1`: `5 > 3` is 1, and `1 > 1` is 0.

### What does this print?
```c
int arr[] = {10, 20, 30};
int *p = arr;
printf("%d ", *p++);
printf("%d\n", *p);
```
- [ ] 11 11
- [x] 10 20
- [ ] 20 20
- [ ] 10 10
> Postfix `++` binds tighter than `*`: `*p++` is `*(p++)` — it reads `*p` (10), then advances the pointer.

### What does this print?
```c
unsigned char uc = 200;
uc += 100;
printf("%d\n", uc);
```
- [ ] 300
- [x] 44
- [ ] 255
- [ ] Undefined behavior
> Unsigned arithmetic wraps modulo 2⁸: 300 − 256 = 44. Unsigned wraparound is well defined; signed overflow is not.

### What does `strlen("a\0bc")` return?
- [ ] 4
- [ ] 5
- [x] 1
- [ ] 3
> `strlen` stops at the first `'\0'`. (`sizeof("a\0bc")` would be 5.)

### What is the result of `int i = 5; i = i++ + ++i;` in C?
- [ ] 11
- [ ] 12
- [ ] 13
- [x] Undefined behavior
> `i` is modified twice without sequencing between the modifications; the compiler may produce any result.

### What happens here?
```c
char *p = "hello";
p[0] = 'H';
```
- [ ] p becomes "Hello"
- [ ] It does not compile in C
- [x] Undefined behavior — usually a segfault, because string literals are in read-only memory
- [ ] Only the first character is copied to a new buffer
> A string literal must not be modified. Use `char p[] = "hello";` to get a writable copy. In C++ `char *p = "hello"` does not even compile (it must be `const char *`).

### How many times does this loop run?
```c
for (unsigned i = 3; i >= 0; i--) printf("%u\n", i);
```
- [ ] 3
- [ ] 4
- [x] Forever
- [ ] 0
> An unsigned value is always ≥ 0; after 0, `i--` wraps to 4294967295 and the loop continues.

### What is wrong with this code?
```c
char *copy = malloc(strlen(src));
strcpy(copy, src);
```
- [ ] Nothing
- [x] It allocates one byte too few — the `'\0'` is written past the end
- [ ] `malloc` must be cast to `char *` in C
- [ ] `strcpy` cannot copy into heap memory
> It needs `strlen(src) + 1` bytes. The cast is required in C++ but not in C.

### What are the sizes?
```cpp
std::vector<int> v1(3, 5), v2{3, 5};
```
- [x] v1.size() == 3, v2.size() == 2
- [ ] Both have size 3
- [ ] Both have size 2
- [ ] v1.size() == 2, v2.size() == 3
> Parentheses call the (count, value) constructor → {5, 5, 5}. Braces prefer the `initializer_list` constructor → {3, 5}.

### What does this print?
```cpp
struct B { B() { hi(); } virtual void hi() { cout << "base"; } };
struct D : B { void hi() override { cout << "derived"; } };
D d;
```
- [x] base
- [ ] derived
- [ ] base derived
- [ ] Undefined behavior
> While `B`'s constructor runs, the object is still a `B` (the `D` part does not exist yet), so the virtual call resolves to `B::hi`. The same applies in destructors.

### What does `p->f()` print?
```cpp
struct B { virtual void f(int x = 1) { cout << "base " << x; } };
struct D : B { void f(int x = 2) override { cout << "derived " << x; } };
D d; B *p = &d; p->f();
```
- [ ] derived 2
- [x] derived 1
- [ ] base 1
- [ ] base 2
> The function is chosen at run time (dynamic type `D`), but default arguments are filled in at compile time from the static type `B *`.

### What does this print?
```cpp
struct S { virtual string n() const { return "S"; } };
struct T : S { string n() const override { return "T"; } };
S s = T();
cout << s.n();
```
- [ ] T
- [x] S
- [ ] It does not compile
- [ ] Undefined behavior
> Object slicing: only the `S` part is copied into `s`, and `s` really is an `S`. Polymorphism needs a pointer or a reference.

### What is `m.size()` after this?
```cpp
std::map<std::string, int> m;
if (m["x"] == 0) { }
```
- [ ] 0
- [x] 1
- [ ] It throws std::out_of_range
- [ ] It does not compile
> `operator[]` inserts a value-initialized element when the key is missing. Use `find`, `count` or `contains` (C++20) to test without inserting; `at` throws instead.

### What is `sp.use_count()` at the end?
```cpp
auto sp = std::make_shared<int>(1);
auto sp2 = sp;
{ auto sp3 = sp; }
```
- [ ] 1
- [x] 2
- [ ] 3
- [ ] 0
> `sp3` raised the count to 3, then its destruction at the end of the block brought it back to 2 (`sp` and `sp2`).

### What is `sizeof` of an empty class `struct E {};`?
- [ ] 0
- [x] 1
- [ ] 8
- [ ] It does not compile
> Every complete object must have a non-zero size so that distinct objects have distinct addresses. (An empty base class can take 0 bytes inside a derived class — the empty base optimization.)

### What does this print?
```cpp
int x = 5; int &r = x;
auto a = r;
a = 20;
cout << x;
```
- [x] 5
- [ ] 20
- [ ] It does not compile
- [ ] Undefined behavior
> `auto` deduction drops references, so `a` is a new `int` copy. `auto &a = r;` would modify `x`.

### What does this print?
```cpp
int c = 1;
auto l = [c] { return c; };
c = 9;
cout << l();
```
- [x] 1
- [ ] 9
- [ ] It does not compile
- [ ] Undefined behavior
> A by-value capture copies `c` when the lambda is created. `[&c]` would print 9.

### What is the type of `x` in `auto x = {1};`?
- [ ] int
- [x] std::initializer_list<int>
- [ ] int[1]
- [ ] std::vector<int>
> With copy-list-initialization, `auto` deduces `std::initializer_list`. `auto x{1};` deduces `int` (since C++17).

### What is the type of `true ? 1 : 2.0`, and what does `cout` print for it?
- [ ] int, prints 1
- [x] double, prints 1
- [ ] double, prints 1.0
- [ ] It does not compile
> Both branches are converted to their common type, `double`. The value is 1.0, which `cout` prints as "1" with the default format.

### What does `cout << 'a' + 1;` print?
- [ ] b
- [x] 98
- [ ] a1
- [ ] 'b'
> `'a' + 1` is promoted to `int` (97 + 1). `cout << char('a' + 1)` prints "b".

### What is `st.size()`?
```cpp
std::set<int> st{1, 2, 2, 3};
st.insert(3);
```
- [ ] 5
- [ ] 4
- [x] 3
- [ ] 2
> A `set` stores unique keys; duplicates are ignored (`insert` returns a pair whose `second` is false).

### What does this print?
```cpp
std::map<int, char> m{{3, 'c'}, {1, 'a'}, {2, 'b'}};
for (auto &kv : m) cout << kv.second;
```
- [ ] cab
- [x] abc
- [ ] An unspecified order
- [ ] cba
> `std::map` is ordered by key (a balanced tree). `std::unordered_map` would have an unspecified order.

### What does `std::string("10") < std::string("9")` evaluate to?
- [x] true
- [ ] false
- [ ] It does not compile
- [ ] It throws
> Strings compare lexicographically, character by character: '1' < '9'.

### What does `v.size() - 1 > 0` give for an empty `std::vector<int> v`?
- [ ] false
- [x] true
- [ ] It throws
- [ ] Undefined behavior
> `size()` returns an unsigned `size_t`; 0 − 1 wraps to the maximum value, which is greater than 0. Loops like `for (size_t i = 0; i < v.size() - 1; ++i)` break on empty vectors.

### Which std::string line does not compile?
- [ ] `std::string s = std::string("a") + "b";`
- [ ] `std::string s = "a"; s += "b";`
- [x] `std::string s = "a" + "b";`
- [ ] `std::string s = "a" "b";`
> `"a" + "b"` tries to add two `const char *` pointers. Adjacent literals `"a" "b"` are concatenated by the compiler.

### What happens?
```cpp
struct S {
    int a, b;
    S() : b(1), a(b + 1) {}
};
```
- [ ] a == 2, b == 1
- [x] `a` reads `b` before it is initialized (undefined value)
- [ ] It does not compile
- [ ] a == 1, b == 1
> Members are initialized in declaration order (`a` first), regardless of the order in the initializer list. `-Wall` warns with `-Wreorder`.

### What is the problem with this code?
```cpp
struct Base { ~Base() {} };
struct Derived : Base { std::string s; };
Base *p = new Derived;
delete p;
```
- [ ] None
- [x] Undefined behavior: `Base` needs a virtual destructor
- [ ] It does not compile
- [ ] It leaks only `Base`
> Deleting a derived object through a base pointer requires a virtual destructor; otherwise `~Derived` (and `s`'s destructor) is typically never run.

### What does `std::move(x)` do by itself?
- [ ] It moves `x` into a new object
- [ ] It sets `x` to an empty state
- [x] It only casts `x` to an rvalue reference; nothing is moved until a move constructor/assignment uses it
- [ ] It frees `x`'s memory
> `std::move` is a `static_cast<T&&>`. `auto y = std::move(x);` moves; `std::move(x);` alone does nothing.

### What is the default inheritance access in `class D : B {};`?
- [ ] public
- [ ] protected
- [x] private
- [ ] It depends on B
> For `class` the default is private (for members and for inheritance); for `struct` it is public.

### Which handler runs when a `Derived` (which inherits from `Base`) is thrown?
```cpp
try { throw Derived(); }
catch (Base &) { cout << "base"; }
catch (Derived &) { cout << "derived"; }
```
- [ ] derived
- [x] base
- [ ] Both
- [ ] It calls std::terminate
> Handlers are tried in order and the first match wins; `Base &` matches a `Derived`. Put the most derived handlers first (compilers warn about this).

### Given `std::unique_ptr<int> u1`, which line does not compile?
- [ ] `auto u2 = std::move(u1);`
- [x] `auto u2 = u1;`
- [ ] `std::unique_ptr<int> u2(u1.release());`
- [ ] `auto u2 = std::make_unique<int>(*u1);`
> `unique_ptr` is move-only: its copy constructor is deleted.

### Why is this copy constructor wrong?
```cpp
struct A { A(A other) { } };
```
- [ ] It is fine
- [x] Passing by value would itself need the copy constructor → infinite recursion, so it does not compile
- [ ] It should return `A &`
- [ ] It must be `explicit`
> A copy constructor must take its parameter by reference, normally `const A &`.

### What does `cout << 3 / 2 * 2.0;` print?
- [ ] 3
- [x] 2
- [ ] 1.5
- [ ] 3.0
> Left to right: `3 / 2` is integer division (1), then `1 * 2.0` = 2.0, printed as "2".

### Can functions be overloaded by return type only, e.g. `int f();` and `double f();`?
- [ ] Yes
- [x] No — it is a redeclaration error
- [ ] Only if one of them is `const`
- [ ] Only for member functions
> The return type is not part of the overload signature. (A `const` member function is a different overload because of `this`, not because of the return type.)

### What is the result of `delete p;` when `p = new int[10];`?
- [ ] It frees all 10 ints
- [ ] It frees only the first int
- [x] Undefined behavior
- [ ] It does not compile
> Memory from `new[]` must be released with `delete[]`, and memory from `new` with `delete`.

### How many lines of "hi" are printed?
```c
fork();
fork();
printf("hi\n");
```
- [ ] 2
- [ ] 3
- [x] 4
- [ ] 1
> Each fork doubles the number of processes: 1 → 2 → 4, and every process prints.

### How many processes (including the original) exist after `for (int i = 0; i < 3; i++) fork();`?
- [ ] 3
- [ ] 4
- [ ] 6
- [x] 8
> Every process continues the loop after forking: 2³ = 8.

### How many processes (including the original) print "x"?
```c
fork() && fork();
printf("x\n");
```
- [ ] 2
- [x] 3
- [ ] 4
- [ ] 1
> In the first child, `fork()` returns 0, so `&&` short-circuits and it does not fork again. The parent forks a second time → parent + 2 children = 3.

### How many times is "hi" printed, even on a terminal?
```c
printf("hi");
fork();
printf("\n");
```
- [ ] 1
- [x] 2
- [ ] 0
- [ ] It depends on the scheduler
> "hi" has no newline, so it is still in the stdio buffer at `fork()`. The buffer is copied into the child, and both processes flush it.

### `printf("hi\n"); fork();` prints "hi" once on a terminal. What about `./prog > out.txt`?
- [ ] Once
- [x] Twice
- [ ] Zero times
- [ ] Once, but the file is truncated
> stdout is line buffered only when it is a terminal. Redirected to a file or pipe it is fully buffered, so "hi\n" is still in the buffer at `fork()` and both processes write it. Call `fflush(stdout)` before `fork()`.

### What does the parent print?
```c
int x = 1;
if (fork() == 0) { x = 2; exit(0); }
wait(NULL);
printf("%d\n", x);
```
- [x] 1
- [ ] 2
- [ ] It depends on which process runs first
- [ ] Undefined behavior
> After `fork()` the child has its own copy of the memory (copy-on-write). Changing `x` in the child does not affect the parent.

### A child calls `exit(256)`. What is `WEXITSTATUS(status)` in the parent?
- [ ] 256
- [ ] 255
- [x] 0
- [ ] -1
> Only the low 8 bits of the exit status are passed to the parent: 256 & 0xFF = 0, which looks like success.

### What does `open()` return here?
```c
close(1);
int fd = open("log.txt", O_WRONLY | O_CREAT, 0644);
```
- [ ] 3
- [x] 1
- [ ] 0
- [ ] -1
> `open` always returns the lowest free descriptor. With 1 closed, the file becomes stdout — this is how shells implemented redirection before `dup2`.

### What does `wait(NULL)` return when the process has no children left?
- [ ] 0
- [x] -1, with errno set to ECHILD
- [ ] It blocks forever
- [ ] The PID of init
> `wait` fails immediately when there are no children to wait for.

### When does `execvp()` return?
- [ ] Always, after the new program finishes
- [ ] Always, with the new program's exit status
- [x] Only on failure
- [ ] Never
> On success the process image is replaced, so there is nothing to return to. Code after `exec` runs only if it failed.

### Which signals can neither be caught nor ignored? (select all that apply)
- [x] SIGKILL
- [x] SIGSTOP
- [ ] SIGTERM
- [ ] SIGINT
- [ ] SIGSEGV
> SIGKILL and SIGSTOP always act; the others can be handled. (A handler for SIGSEGV that returns normally just re-runs the faulting instruction.)

### Which signal does `kill 1234` send?
- [ ] SIGKILL (9)
- [x] SIGTERM (15)
- [ ] SIGINT (2)
- [ ] SIGHUP (1)
> SIGTERM is the default and can be caught, so a program can clean up. `kill -9` is SIGKILL.

### What does `false | true; echo $?` print in bash (default settings)?
- [ ] 1
- [x] 0
- [ ] 127
- [ ] It prints nothing
> A pipeline's exit status is that of its last command. With `set -o pipefail` it would be 1.

### Where does stderr go in `ls /nonexistent 2>&1 >out.txt`?
- [ ] To out.txt
- [x] To the terminal
- [ ] To /dev/null
- [ ] Nowhere
> Redirections are processed left to right: `2>&1` copies the current stdout (the terminal) into fd 2, then `>out.txt` changes only fd 1. Write `>out.txt 2>&1` to send both to the file.

### What does this print in bash?
```bash
x=1
echo a | read x
echo $x
```
- [ ] a
- [x] 1
- [ ] An empty line
- [ ] An error
> Each part of a pipeline runs in a subshell, so `read` sets `x` in a child process that then exits. (`shopt -s lastpipe` or `read x <<< a` avoids this.)

### What does this print in bash?
```bash
a=5
b=$a+1
echo $b
```
- [ ] 6
- [x] 5+1
- [ ] An error
- [ ] 51
> Shell variables are strings; `$a+1` is just text. Use `b=$((a + 1))`.

### A file contains the lines `foo foo`, `bar` and `foo`. What does `grep -c foo file` print?
- [ ] 3
- [x] 2
- [ ] 1
- [ ] 0
> `-c` counts matching lines. `grep -o foo file | wc -l` counts occurrences (3).

### What is the link count (`stat -c %h`) of a newly created empty directory?
- [ ] 0
- [ ] 1
- [x] 2
- [ ] 3
> Its entry in the parent plus its own `.` entry. Each subdirectory adds one more (its `..`).

### A process has `big.log` open and you run `rm big.log`. When is the disk space freed?
- [ ] Immediately
- [x] When the last open descriptor to the file is closed
- [ ] At the next reboot
- [ ] Never, until `fsck` runs
> `rm` only removes a directory entry (it calls `unlink`). The inode and its data stay until the link count and the open count are both 0. `lsof +L1` lists such deleted-but-open files.

### Why does `./script.sh` containing `cd /tmp` not change your shell's directory?
- [ ] `cd` is not allowed in scripts
- [x] The script runs in a child process, and a child cannot change its parent's working directory
- [ ] The script needs `sudo`
- [ ] `cd` only works with absolute paths
> The working directory is per-process. Use `source script.sh` (or `. script.sh`) to run it in the current shell.

### Which permission do you need on a directory to `cd` into it?
- [ ] r
- [ ] w
- [x] x
- [ ] s
> On a directory, `r` lets you list the names, `w` lets you create/delete entries (together with `x`), and `x` lets you traverse it and access entries by name.

### `chmod 640 f` gives which permissions?
- [ ] rw-r--r--
- [x] rw-r-----
- [ ] rwxr-----
- [ ] r--rw----
> 6 = rw-, 4 = r--, 0 = ---, for owner, group and others.

### `main()` creates a thread and then returns without calling `pthread_join`. What happens to the thread?
- [ ] It keeps running until it finishes
- [x] It is killed, because returning from main calls exit(), which ends the whole process
- [ ] It becomes a zombie thread
- [ ] main waits for it automatically
> To end only the main thread and let the others continue, call `pthread_exit(NULL)` in `main`.

### Which of these do threads of the same process NOT share?
- [ ] Global variables
- [ ] The heap
- [ ] Open file descriptors
- [x] Their stack and registers
> Each thread has its own stack and register set (including its own `errno`, which is thread-local). Everything else in the address space is shared.

### A child process has exited, but the parent has not called `wait()`. What is the child?
- [ ] An orphan
- [x] A zombie
- [ ] A daemon
- [ ] Still running
> A zombie keeps its process table entry (PID and exit status) until the parent waits for it. An orphan is the opposite: a running child whose parent died; it is adopted by init (PID 1) or a subreaper.

### Which `nice` value gives a process the highest priority?
- [ ] 19
- [ ] 0
- [x] -20
- [ ] 99
> Lower nice values mean higher priority (range -20 … 19); only root can lower the nice value.
