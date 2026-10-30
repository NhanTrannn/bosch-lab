---
tags: [cpp, types, operators, control-flow, strings]
---

# C++ basics: variables, operators, control flow, strings

## Theory

Follows the Udemy course *Beginning C++ Programming*, sections 4–10. Labs: `cpp/17` … `cpp/20`.

### Kinds of errors (section 4)
| Kind | Detected when | Example |
| --- | --- | --- |
| Compiler error (syntax/semantic) | at compile time | missing `;`, using an undeclared variable, assigning a `string` to an `int` |
| Compiler warning | at compile time, still produces an executable | uninitialized variable, signed/unsigned comparison, unused variable |
| Linker error | at link time | function declared but never defined (`undefined reference`), defined twice (`multiple definition`) |
| Runtime error | while running | division by zero, opening a missing file, dereferencing a null pointer → crash |
| Logic error | nobody reports it, the result is just wrong | using `>` instead of `>=`, integer division where real division was needed |

Always enable `-Wall -Wextra` and treat warnings as latent bugs.

### Program structure (section 5)
- `#include <iostream>`: a **preprocessor** directive, processed before compilation; it pastes the header's contents in. `<...>` searches the system directories, `"..."` searches the current directory first.
- Comments: `//` single line, `/* ... */` multi-line (cannot nest). Comments are stripped by the preprocessor.
- `main()`: the entry point; `int main()` or `int main(int argc, char *argv[])`. `argv[0]` is the program name. A missing `return` in main means return 0; returning 0 means success.
- Namespaces avoid name clashes. `std::cout`, `using std::cout;` (one name), `using namespace std;` (the whole namespace; avoid in headers).
- `cout << ` (insertion operator), `cin >> ` (extraction operator). `cin >>` skips leading whitespace and stops at whitespace. `endl` = newline + flush; `'\n'` is just a newline.
- `cerr`: unbuffered; `clog`: buffered; both write to stderr.

### Variables and constants (section 6)
- A variable: name + type + storage. Names start with a letter or `_`, are case-sensitive and cannot be keywords.
- Three ways to initialize: `int a = 5;` (C-like), `int a(5);` (constructor), `int a{5};` (list/uniform, C++11, **blocks narrowing**: `int x{3.5}` is an error).
- An uninitialized local holds garbage; globals/statics are zero-initialized.
- Global variables: declared outside every function, visible everywhere after the declaration. Avoid them.
- Fundamental types: `char` (1 byte), `short` (≥ 2), `int` (≥ 2, usually 4), `long` (≥ 4; 8 on 64-bit Linux), `long long` (≥ 8), `float` (4, ~7 digits), `double` (8, ~15 digits), `long double`, `bool` (usually 1 byte).
- `signed`/`unsigned`: unsigned has no negative values and wraps around on overflow (well-defined); signed overflow is UB.
- `sizeof` is an **operator** (evaluated at compile time) returning `size_t`. Type limits: `<climits>` (`INT_MAX`), `<cfloat>`, `std::numeric_limits<int>::max()`.
- Constants: literals (`12`, `12U`, `12L`, `12.0f`, `'A'`, `"abc"`), `const` (cannot change after initialization, must be initialized immediately), `constexpr` (evaluated at compile time), `enum`, `#define` (text substitution, untyped, **avoid** in C++).

### Arrays and vectors (section 7)
- Arrays: fixed size known at compile time, contiguous elements, indexes 0 … n-1, **no bounds checking**. `int a[5] {1, 2};` → the rest are 0. `int a[] {1, 2, 3};` → size 3 deduced.
- An array name is the address of its first element. Arrays cannot be assigned (`a = b` is an error).
- Multidimensional arrays: `int m[3][4];` stored row by row (row-major).
- `std::vector` (`#include <vector>`): a dynamic array that manages its own memory. `vector<int> v(5);` = 5 elements of 0; `vector<int> v{5};` = one element with value 5; `vector<int> v(5, 1);` = 5 elements of 1.
- `v[i]` is unchecked, `v.at(i)` checks and throws `std::out_of_range`. `push_back`, `size()`, `vector<vector<int>>` for 2D.

### Expressions and operators (section 8)
- Expression: something that computes a value. Statement: a complete instruction ending with `;`.
- The assignment `=` returns the left-hand lvalue → `a = b = 5` assigns right to left. `=` is not `==`.
- Arithmetic `+ - * / %`. `int / int` is integer division (truncates toward zero). `%` only works on integers.
- `++i` (increment, then use) and `i++` (use, then increment). Do not modify the same variable twice in one expression (`i = i++ + 1` is UB before C++17).
- Mixed expressions: smaller types are promoted to larger ones (promotion: `short`/`char` → `int`, `int` → `double`). Assigning a double to an int truncates the fraction (demotion). Explicit cast: `static_cast<double>(x)`, C-style `(double)x`.
- Comparison `== != < > <= >=` (C++20 adds `<=>`). Do not compare floating-point numbers with `==`.
- Logical: `!` (highest), `&&`, `||` (lowest). **Short-circuit**: `&&` stops at the first false, `||` stops at the first true.
- Compound assignment `+= -= *= /= %= <<= >>= &= ^= |=`: `a *= b + c` is `a = a * (b + c)`.
- Precedence (high → low): `::` → postfix `a++ a() a[] .` `->` → prefix `++a !a -a *p &a sizeof` casts → `* / %` → `+ -` → `<< >>` → `< <= > >=` → `== !=` → `&` → `^` → `|` → `&&` → `||` → `?:` and assignment `= +=` … → `,`. Assignment and `?:` associate **right to left**, most other operators left to right.

### Control flow (section 9)
- `if`, `if-else`, nested ifs: an `else` binds to the nearest `if` without an else (dangling else) → always use `{}`.
- `switch (expr)`: expr must be an integer, char or enum (not a string or double). `case` labels must be compile-time constants. A missing `break` falls through to the next case. `default` is optional. You cannot declare an initialized variable inside a case without `{}`.
- The conditional operator `cond ? a : b` is an **expression** (it has a value), so it can be used inside `cout`.
- `for (init; cond; update)`: init and update may handle several variables (comma). `for (;;)` is an infinite loop.
- Range-based for (C++11): `for (auto x : v)` copies, `for (auto &x : v)` can modify, `for (const auto &x : v)` reads without copying.
- `while` tests first (may run 0 times); `do { } while (cond);` tests last (runs at least once; note the final `;`).
- `break` leaves the innermost loop/switch; `continue` jumps to the next iteration of the innermost loop.
- Infinite loops: `while (true)`, `for (;;)`; leave with `break`. Common for menu loops and event loops.

### Characters and strings (section 10)
- `<cctype>`: `isalpha`, `isdigit`, `isalnum`, `isspace`, `isupper`, `islower`, `ispunct`, `toupper`, `tolower`. The `is...` functions return a non-zero int (not necessarily 1).
- C-style string: a `char` array terminated by `'\0'`. `char s[] = "Hi";` has size 3. A string literal `"Hi"` is a `const char[3]` and must not be modified.
- `<cstring>`: `strlen` (does not count `'\0'`), `strcpy`, `strcat` (no overflow checks), `strcmp` (0 = equal, < 0, > 0), `strncpy`. `<cstdlib>`: `atoi`, `atof`.
- `std::string` (`#include <string>`): manages its own memory, compares with `== < >` lexicographically, concatenates with `+` (at least one side must be a `std::string`: `"a" + "b"` is an error).
- String member functions: `size()`/`length()`, `substr(pos, len)`, `find(s)` (not found → `string::npos`), `rfind`, `erase(pos, len)`, `insert(pos, s)`, `replace`, `clear()`, `empty()`, `at(i)` (bounds-checked), `c_str()` (get a `const char*`), `std::stoi`, `std::to_string`.
- `cin >> s` reads one word; `getline(cin, s)` reads a whole line (dropping the `'\n'`). Calling `cin >> n` and then `getline` reads the leftover `'\n'` → an empty string; fix with `cin.ignore()` or `cin >> ws`.

## Quiz

### You forgot to define a function that is declared and called. At which stage does the error appear? {#cpp-basics-001}
- [ ] Preprocessor
- [ ] Compilation (compiler error)
- [x] Linking (linker error: undefined reference)
- [ ] At run time
> The declaration is enough for the compiler to compile the call. Only the linker, when combining the object files, finds that there is no definition.

### An average-calculating program prints 2 instead of 2.33. What kind of error is this? {#cpp-basics-002}
```cpp
int a = 1, b = 2, c = 4;
double avg = (a + b + c) / 3;
```
- [ ] Compiler error
- [ ] Runtime error
- [x] Logic error
- [ ] Linker error
> The code is valid and runs fine; the result is just wrong because of integer division. Fix it with `/ 3.0` or `static_cast<double>(a + b + c) / 3`.

### Who processes `#include`? {#cpp-basics-003}
- [x] The preprocessor, before compilation
- [ ] The compiler, during compilation
- [ ] The linker
- [ ] The operating system when the program runs
> The preprocessor handles every line starting with `#` (include, define, ifdef) and removes comments, then hands the result to the compiler.

### How do `#include <file>` and `#include "file"` differ? {#cpp-basics-004}
- [ ] No difference
- [x] `"file"` searches the source file's directory first, then the system directories; `<file>` only searches the system/-I directories
- [ ] `<file>` is for C, `"file"` is for C++
- [ ] `"file"` only works for .cpp files
> Convention: library headers use `<>`, project headers use `""`.

### Why should you not write `using namespace std;` in a header file? {#cpp-basics-005}
- [ ] It makes the program slower
- [x] Every file that includes the header gets all of std's names pulled in, which easily causes clashes
- [ ] Headers cannot use namespaces
- [ ] That syntax is only valid inside main
> `using` costs nothing at run time. The problem is namespace pollution everywhere the header is included.

### How does `std::endl` differ from `'\n'`? {#cpp-basics-006}
- [ ] `endl` prints two newlines
- [x] `endl` prints a newline and flushes the stream's buffer
- [ ] `'\n'` does not produce a newline on Linux
- [ ] No difference
> Flushing all the time slows down heavy output. Use `'\n'` unless you need the output to appear immediately.

### Which initialization is a compile error? {#cpp-basics-007}
- [ ] `int a = 3.7;`
- [ ] `int a(3.7);`
- [x] `int a{3.7};`
- [ ] `double a{3};`
> List initialization `{}` forbids narrowing (double → int loses data). `=` and `()` just truncate, at most with a warning. `double a{3}` widens (int → double), so it is fine.

### What is the value of an uninitialized local `int x;`? {#cpp-basics-008}
- [ ] Always 0
- [x] Indeterminate (garbage); reading it is undefined behavior
- [ ] -1
- [ ] Compile error
> Only globals/statics are zero-initialized automatically. Compilers usually warn with `-Wuninitialized`/`-Wmaybe-uninitialized`.

### What is `sizeof`? {#cpp-basics-009}
- [ ] A function in `<cstddef>`
- [x] An operator whose result has type `size_t` and is usually computed at compile time
- [ ] A macro
- [ ] A member function
> Because it is an operator, `sizeof x` (no parentheses) is valid for variables; types need parentheses: `sizeof(int)`.

### Which statements about `const` and `constexpr` are TRUE? (select all that apply) {#cpp-basics-010}
- [x] A `const` variable must be initialized when declared
- [x] `constexpr` requires a value computable at compile time
- [ ] `const int n = rand();` is an error
- [x] `constexpr int n = 10;` can be used as an array size
> `const` only forbids modification; its value can come from run time (`rand()` is fine). `constexpr` is stronger: it must be a compile-time constant.

### Why does C++ recommend `const`/`constexpr` instead of `#define PI 3.14`? {#cpp-basics-011}
- [ ] `#define` is slower
- [x] `#define` is pure text substitution: no type, no scope, invisible in the debugger
- [ ] `#define` cannot be used for floating-point values
- [ ] `const` is stored in a register
> Macros are replaced before the compiler sees them, so there is no type checking, no scope, and missing parentheses cause bugs.

### Given `int a[5] {1, 2};`, what is `a[4]`? {#cpp-basics-012}
- [ ] Garbage
- [x] 0
- [ ] 2
- [ ] Compile error
> With an initializer list, elements that are not listed are initialized to 0.

### How do `vector<int> v(5);` and `vector<int> v{5};` differ? {#cpp-basics-013}
- [ ] They are the same: both have 5 elements
- [x] `(5)` is 5 elements of value 0; `{5}` is one element with value 5
- [ ] `(5)` is one element with value 5; `{5}` is 5 elements
- [ ] `{5}` is a compile error
> Braces prefer the constructor taking an `initializer_list`.

### What does `v.at(10)` do on a vector with 3 elements? {#cpp-basics-014}
- [ ] Returns 0
- [ ] Undefined behavior
- [x] Throws `std::out_of_range`
- [ ] Grows the vector
> `at()` checks bounds. `v[10]` does not → UB.

### What are `-7 / 2` and `-7 % 2` in C++11? {#cpp-basics-015}
- [ ] -4 and 1
- [x] -3 and -1
- [ ] -3 and 1
- [ ] -4 and -1
> Integer division truncates toward zero, and `(a / b) * b + a % b == a` → `-7 % 2 = -7 - (-3 * 2) = -1`.

### What does this print? {#cpp-basics-016}
```cpp
int i = 5;
int j = i++ + 10;
cout << i << ' ' << j;
```
- [ ] 5 15
- [x] 6 15
- [ ] 6 16
- [ ] 5 16
> `i++` yields the old value (5) for computing j = 15, then i becomes 6.

### With a = 4, what is a after `a *= 2 + 3;`? {#cpp-basics-017}
- [ ] 11
- [x] 20
- [ ] 8
- [ ] 14
> Compound assignment has the lowest precedence: `a = a * (2 + 3)` = 20.

### What is `(5 > 3 > 1)`? {#cpp-basics-018}
- [ ] true, because 5 > 3 and 3 > 1
- [x] false
- [ ] Compile error
- [ ] Undefined behavior
> Evaluated left to right: `(5 > 3)` = true = 1, then `1 > 1` = false.

### What does this print? {#cpp-basics-019}
```cpp
int x = 0, y = 0;
if (x != 0 && ++y > 0) {}
if (x == 0 || ++y > 0) {}
cout << y;
```
- [x] 0
- [ ] 1
- [ ] 2
- [ ] Undefined
> Both short-circuit: the left side of `&&` is false, the left side of `||` is true → `++y` never runs.

### What is `true || false && false`? {#cpp-basics-020}
- [x] true
- [ ] false
- [ ] Compile error
- [ ] Compiler-dependent
> `&&` binds tighter than `||`: `true || (false && false)` = true.

### What is `static_cast<double>(7 / 2)`? {#cpp-basics-021}
- [ ] 3.5
- [x] 3.0
- [ ] 4.0
- [ ] Compile error
> The parenthesized expression is integer division (3), which is then cast to double. For 3.5, cast first: `static_cast<double>(7) / 2`.

### What does `cout << 'A' + 1;` print? {#cpp-basics-022}
- [ ] B
- [x] 66
- [ ] A1
- [ ] Compile error
> `char` is promoted to `int` in the addition → the result is the number 66. To print 'B', cast back: `char('A' + 1)`.

### Which type can NOT be used as a `switch` condition? {#cpp-basics-023}
- [ ] `char`
- [ ] `enum`
- [x] `std::string`
- [ ] `long`
> switch only accepts integer types (including char and bool) and enums. Strings and floating-point values need an if-else chain.

### What does this print? {#cpp-basics-024}
```cpp
int n = 2;
switch (n) {
case 1: cout << "A";
case 2: cout << "B";
case 3: cout << "C"; break;
default: cout << "D";
}
```
- [ ] B
- [x] BC
- [ ] BCD
- [ ] BD
> There is no `break` after case 2, so execution falls through into case 3, and the `break` in case 3 stops it.

### What does this print? {#cpp-basics-025}
```cpp
int x = 1, y = 0;
if (x)
    if (y) cout << "A";
else
    cout << "B";
```
- [ ] A
- [x] B
- [ ] Nothing
- [ ] AB
> The `else` belongs to the nearest `if (y)`; indentation means nothing to the compiler.

### Which loop always runs its body at least once? {#cpp-basics-026}
- [ ] `for`
- [ ] `while`
- [x] `do-while`
- [ ] range-based for
> do-while tests the condition at the end of each iteration.

### What is v after this code? {#cpp-basics-027}
```cpp
vector<int> v{1, 2, 3};
for (auto x : v) x *= 2;
```
- [x] {1, 2, 3}
- [ ] {2, 4, 6}
- [ ] Compile error
- [ ] {1, 4, 9}
> `auto x` is a copy. To modify elements you need `auto &x`.

### In nested loops, `break` leaves: {#cpp-basics-028}
- [x] Only the innermost loop containing it
- [ ] All loops
- [ ] The whole function
- [ ] The outermost loop
> To leave several loops use a flag, move the loops into a function and `return`, or `goto`.

### For `char s[] = "Hello";`, what are `sizeof(s)` and `strlen(s)`? {#cpp-basics-029}
- [ ] 5 and 5
- [x] 6 and 5
- [ ] 5 and 6
- [ ] 8 and 5
> The array holds 5 characters + `'\0'`. strlen stops before `'\0'`.

### What is the correct way to compare the contents of two C-style strings `s1` and `s2`? {#cpp-basics-030}
- [ ] `s1 == s2`
- [x] `strcmp(s1, s2) == 0`
- [ ] `strcmp(s1, s2) == 1`
- [ ] `s1.compare(s2)`
> `==` on `char*`/arrays compares addresses. strcmp returns 0 when equal, negative/positive when less/greater.

### Which line is a compile error? {#cpp-basics-031}
- [ ] `string s = string("a") + "b";`
- [ ] `string s = "a"; s += "b";`
- [x] `string s = "a" + "b";`
- [ ] `string s = "a" + string("b");`
> `"a" + "b"` adds two `const char*` pointers, which is not allowed. At least one side must be a `std::string`.

### What does `s.find("xyz")` return when the text is not found? {#cpp-basics-032}
- [ ] -1 as an int
- [ ] 0
- [x] `std::string::npos`
- [ ] Throws an exception
> `npos` is the largest `size_t` value. Always compare `if (s.find(x) != string::npos)`.

### What is `string("Zebra") < string("apple")`? {#cpp-basics-033}
- [x] true
- [ ] false
- [ ] Compile error
- [ ] Undefined
> Characters are compared by ASCII code: 'Z' (90) < 'a' (97).

### The user types `42` and presses Enter. What does this code do? {#cpp-basics-034}
```cpp
int n; string name;
cin >> n;
getline(cin, name);
```
- [ ] Waits for a name
- [x] `name` is an empty string; it does not wait
- [ ] `name` is "42"
- [ ] Runtime error
> `cin >> n` leaves the `'\n'` in the buffer and getline reads it immediately. Call `cin.ignore()` (or `getline(cin >> ws, name)`) first.
