---
tags: [cpp, functions, pointers, references]
---

# Functions, pointers and references

## Theory

Follows the Udemy course *Beginning C++ Programming*, sections 11–12. Labs: `cpp/21_functions`, `cpp/22_pointers_refs`; see also `c/02`, `c/03`, `c/13`.

### Functions (section 11)
- A **prototype** (declaration) tells the compiler the name, return type and parameter types. The definition must appear exactly once in the whole program (ODR). Prototypes usually go in `.h`, definitions in `.cpp`.
- Parameters are the variables in the definition; arguments are the values passed in a call.
- **Pass by value** (default): the function gets a copy; changing it does not affect the original; copying large objects is expensive.
- **Pass by reference** `T&`: the function works on the original, no copy. Use `const T&` when only reading (cheap, safe, and accepts temporaries).
- **Pass by pointer** `T*`: passes an address; may be `nullptr`. The pointer itself is still passed by value.
- `return`: a `void` function may omit it. Never return a reference/pointer to a local variable (dangling).
- **Default arguments**: written only in the declaration (prototype), not repeated in the definition; must be the **last** parameters: `void f(int a, int b = 1, int c = 2);`.
- **Overloading**: same name, different parameter lists (count/types/order). You **cannot** overload on the return type alone. Ranking: exact match → promotion (`char/short → int`, `float → double`) → standard conversion (`int → double`, `const char* → bool`) → user-defined conversion (constructors, conversion operators). Two candidates of the same rank → **ambiguous** error.
- An array passed to a function **decays** to a pointer to its first element: `void f(int a[])` is exactly `void f(int *a)`, so the size is lost → pass it separately. `const int a[]` keeps the function from modifying the array.
- **Scope**: local (a `{}` block), global, static local (initialized once, lives for the whole program but is visible only in the function).
- **How calls work**: each call pushes a **stack frame** (activation record) holding the parameters, locals and return address. Returning pops the frame. The stack is limited (8 MB by default on Linux) → too-deep recursion causes a **stack overflow**.
- `inline`: asks the compiler to insert the function body at the call site to avoid call overhead; the compiler decides. Inline functions may be defined in headers without breaking the ODR.
- **Recursion**: needs a base case. Examples: factorial, Fibonacci (the naive version is O(2ⁿ)). Any recursion can be rewritten as a loop.

### Pointers (section 12)
- A pointer is a variable holding an **address**. `int *p;` (uninitialized → garbage address). Initialize it right away: `int *p = nullptr;` (C++11, replaces `NULL`/`0`).
- `&x`: take the address. `*p`: dereference (access the value at that address). `p->m` = `(*p).m`.
- All data pointers have the same `sizeof` (8 bytes on 64-bit), regardless of the pointed-to type.
- Pointer types must match: `double *p = &int_var;` is a compile error.
- **Dynamic allocation**: `new T` / `delete p`; `new T[n]` / `delete[] p`. Do not mix `new[]` with `delete`, nor `new` with `free`. The memory lives on the **heap** until it is deleted.
- **Arrays and pointers**: `a[i]` ≡ `*(a + i)`. `p + 1` advances by `sizeof(*p)` bytes. The difference of two pointers (into the same array) = the number of elements between them (`ptrdiff_t`). `==` on pointers compares addresses.
- **const and pointers**: `const int *p` (pointer to const: cannot modify `*p`), `int *const p` (const pointer: cannot change `p`), `const int *const p` (both). Read right to left.
- Pass a pointer to a function so it can modify the original (the C way). For a function to change **the pointer itself**, pass `int *&p` or `int **p`.
- Returning a pointer from a function: fine if it points to memory that is still alive (a pointer parameter, `new`'ed memory, a static variable); **never** the address of a local.
- **Common bugs**: uninitialized (wild) pointers, **dangling pointers** (to deleted memory or a variable out of scope), dereferencing `nullptr` (segfault), **memory leaks** (losing the pointer to undeleted memory), double delete. `delete nullptr` is safe → set `p = nullptr` after deleting.

### References
- `int &r = x;` creates another name for x (an alias). It **must be initialized**, **cannot be null** and **cannot be reseated**: `r = y` assigns y's value to x.
- `sizeof(r)` = `sizeof(x)`; `&r == &x`.
- Used for parameters (avoid copies, modify the original) and in range-for (`for (auto &x : v)`).
- `const T&` can bind to a temporary (rvalue): `const int &r = 5;` is valid, `int &r = 5;` is not.
- **lvalue**: has a name and an address, can appear on the left of an assignment. **rvalue**: a temporary (`5`, `x + 1`, a function result returned by value). C++11 adds rvalue references `T&&` (see move semantics).

| | Pointer | Reference |
| --- | --- | --- |
| Null | possible | no |
| Initialization | optional | required |
| Rebind to another object | yes | no |
| Pointer arithmetic | yes | no |
| Access syntax | `*p`, `p->m` | like a normal variable |
| Use when | the object may be absent, must be reseated, dynamic arrays | parameters/aliases that are always valid |

## Quiz

### Can you overload two functions that differ only in parameter names? {#cpp-func-001}
```cpp
void f(int a);
void f(int b);
```
- [ ] Yes, because the parameter names differ
- [x] No, they are the same function (a redeclaration)
- [ ] Yes, if you add `inline`
- [ ] It is a linker error
> A function's signature is its name plus its parameter types; parameter names do not count.

### Given the overloads below, which one does `print("hi")` call? {#cpp-func-002}
```cpp
void print(bool);
void print(const std::string &);
```
- [x] `print(bool)`
- [ ] `print(const std::string &)`
- [ ] Ambiguous error
- [ ] Compile error: no matching overload
> `const char*` → `bool` is a standard conversion, which beats `const char*` → `std::string` (a user-defined conversion). A real trap; see lab 21.

### With `void f(int); void f(double);`, what happens with `f(5L)` (a long)? {#cpp-func-003}
- [ ] Calls f(int)
- [ ] Calls f(double)
- [x] Compile error: ambiguous
- [ ] Calls f(int) because long is closer to int
> long → int and long → double are both standard conversions of the same rank, so the compiler cannot choose.

### With `void f(int); void f(double);`, which one does `f(3.5f)` call? {#cpp-func-004}
- [ ] f(int)
- [x] f(double)
- [ ] Ambiguous
- [ ] Compile error
> float → double is a **promotion**, which beats float → int (a standard conversion).

### Which default-argument declaration is valid? {#cpp-func-005}
- [ ] `void f(int a = 1, int b);`
- [x] `void f(int a, int b = 2, int c = 3);`
- [ ] `void f(int a = 1, int b, int c = 3);`
- [ ] `void f(int = 1, int);`
> Default arguments must be consecutive at the end of the parameter list.

### The header has the prototype `void f(int x = 5);`. How should the definition in the .cpp be written? {#cpp-func-006}
- [ ] `void f(int x = 5) { ... }`
- [x] `void f(int x) { ... }`
- [ ] `void f(int x = 0) { ... }`
- [ ] `void f() { ... }`
> Writing the default in both places gives "redefinition of default argument". Put it only in the declaration that callers see.

### What does this print? {#cpp-func-007}
```cpp
int counter() { static int c = 0; return ++c; }
int main() { counter(); counter(); cout << counter(); }
```
- [ ] 1
- [ ] 0
- [x] 3
- [ ] Undefined
> A static local is initialized only on the first call and keeps its value between calls.

### Inside `void f(int arr[])`, what is `sizeof(arr)` on 64-bit Linux? {#cpp-func-008}
- [ ] The size of the whole array passed in
- [x] 8 (the size of a pointer)
- [ ] 4
- [ ] Compile error
> An array parameter decays to `int*`. gcc even warns with `-Wsizeof-array-argument`. Pass the size as a separate parameter.

### What is the best parameter declaration for a function that only reads a large `std::vector<int>`? {#cpp-func-009}
- [ ] `void f(std::vector<int> v)`
- [ ] `void f(std::vector<int> &v)`
- [x] `void f(const std::vector<int> &v)`
- [ ] `void f(std::vector<int> *v)`
> No copy, no modification, and it also accepts temporaries.

### What happens on every function call? {#cpp-func-010}
- [ ] The function is copied to the heap
- [x] A stack frame holding the parameters, locals and return address is pushed onto the call stack
- [ ] Global variables are saved
- [ ] A new thread is created
> When the function returns the frame is popped, so its locals are no longer valid.

### A recursive function without a base case causes: {#cpp-func-011}
- [ ] A compile error
- [ ] A harmless infinite loop
- [x] A stack overflow (usually a segfault)
- [ ] A heap memory leak
> Each call takes another stack frame until the stack runs out.

### What does `inline` on a function mean? {#cpp-func-012}
- [ ] The compiler must insert the body at every call site
- [x] A hint to inline the body, and permission to define the function in a header without breaking the ODR
- [ ] The function runs on its own thread
- [ ] The function cannot be recursive
> The compiler may ignore the hint (large or recursive functions) or inline functions without the keyword when optimizing.

### Which function returns a dangerous (dangling) value? {#cpp-func-013}
- [ ] `int f() { int x = 5; return x; }`
- [x] `int &f() { int x = 5; return x; }`
- [ ] `int *f() { return new int{5}; }`
- [ ] `int *f() { static int x = 5; return &x; }`
> Returning a reference to a local: the variable is destroyed when the function returns. Returning by value is always safe; pointers to `new`'ed memory or statics are still alive.

### What does this print? {#cpp-func-014}
```cpp
int a = 10, b = 20;
int &r = a;
r = b;
r = 5;
cout << a << ' ' << b;
```
- [ ] 10 5
- [x] 5 20
- [ ] 20 5
- [ ] 5 5
> A reference cannot be reseated: `r = b` assigns 20 to a, then `r = 5` assigns 5 to a. b is unchanged.

### Which statements about references are TRUE? (select all that apply) {#cpp-func-015}
- [x] They must be initialized when declared
- [ ] They can be set to nullptr
- [x] They cannot be changed to refer to another object
- [x] `const int &r = 5;` is valid
> Only `const T&` (or `T&&`) can bind to a temporary like 5.

### What about `int &r = 5;`? {#cpp-func-016}
- [ ] Valid, r = 5
- [x] Compile error: cannot bind a non-const lvalue reference to an rvalue
- [ ] Undefined behavior
- [ ] Valid but with a warning
> 5 is an rvalue. `const int &r = 5;` works: the compiler creates a temporary that lives as long as r.

### What does this print? {#cpp-func-017}
```cpp
void reset(int *p) { p = nullptr; }
int main() {
    int x = 1; int *q = &x;
    reset(q);
    cout << (q == nullptr);
}
```
- [x] 0
- [ ] 1
- [ ] Segfault
- [ ] Compile error
> The pointer is passed by value: the function only changes its copy. To change q, use `int *&p` or `int **p`.

### Given `int a[] = {10, 20, 30}; int *p = a;`, what are `*p + 1` and `*(p + 1)`? {#cpp-func-018}
- [ ] 20 and 20
- [x] 11 and 20
- [ ] 11 and 11
- [ ] 20 and 11
> `*` binds tighter than `+`: `*p + 1` = 10 + 1. `*(p + 1)` is the second element.

### Given `double d[10]; double *p = &d[2], *q = &d[7];`, what is `q - p`? {#cpp-func-019}
- [ ] 40
- [x] 5
- [ ] 20
- [ ] Compile error
> Pointer differences count elements, not bytes.

### Which declaration lets you point the pointer elsewhere but NOT modify the value it points to? {#cpp-func-020}
- [x] `const int *p`
- [ ] `int *const p`
- [ ] `const int *const p`
- [ ] `int const *const p`
> Read right to left: `p` is a pointer to `const int`. `int const *p` is exactly the same as `const int *p`.

### Which allocation/deallocation pair is CORRECT? {#cpp-func-021}
- [ ] `int *p = new int[10]; delete p;`
- [x] `int *p = new int[10]; delete[] p;`
- [ ] `int *p = new int; free(p);`
- [ ] `int *p = (int*)malloc(4); delete p;`
> new goes with delete, new[] with delete[], malloc with free. Mixing them is undefined behavior.

### What is wrong with this code? {#cpp-func-022}
```cpp
void f() {
    int *p = new int[100];
    if (error()) return;
    delete[] p;
}
```
- [ ] Nothing
- [x] A memory leak when `error()` returns true
- [ ] A double free
- [ ] A dangling pointer
> The early return skips the delete. The modern C++ fix: `std::vector<int>` or `std::unique_ptr<int[]>` (RAII).

### `delete p;` and then using `*p` is which kind of bug? {#cpp-func-023}
- [ ] Memory leak
- [x] Dangling pointer (use-after-free), undefined behavior
- [ ] Null pointer dereference
- [ ] Compile error
> After delete, set `p = nullptr`; using nullptr crashes immediately instead of silently reading garbage. AddressSanitizer (`-fsanitize=address`) catches this bug.

### What are `sizeof(int*)` and `sizeof(double*)` on Linux x86-64? {#cpp-func-024}
- [ ] 4 and 8
- [x] 8 and 8
- [ ] 4 and 4
- [ ] It depends on the pointed-to value
> A pointer's size depends only on the architecture, not on the pointed-to type.

### Which expression is an rvalue? {#cpp-func-025}
- [ ] `x`
- [ ] `arr[2]`
- [x] `x + 1`
- [ ] `*p`
> `x + 1` is a temporary with no address. `x`, `arr[2]` and `*p` all designate a specific memory location (lvalues) and can appear on the left of an assignment.

### When should you use a pointer instead of a reference? (select all that apply) {#cpp-func-026}
- [x] When the object may not exist (you need a null value)
- [x] When it must later point to a different object
- [ ] When you want a parameter that is not copied
- [x] When working with dynamic arrays allocated with new[]
> If you only want to avoid copies, a reference (especially `const T&`) is simpler and safer.
