---
tags: [cpp, exceptions, streams, stl]
---

# Smart pointers, exceptions, I/O streams, STL, lambdas, enums

## Theory

Follows the Udemy course *Beginning C++ Programming*, sections 17–21 and 23. Labs: `cpp/26` … `cpp/29`; see also `cpp/06`, `07`, `08`, `11`. The "STL and modern C++" topic above has the container complexity table and move semantics.

### Smart pointers (section 17)
- Problems with raw pointers: forgotten deletes (leaks), double deletes, dangling pointers, unclear ownership, exceptions skipping the delete.
- **RAII** (Resource Acquisition Is Initialization): a resource is tied to an object's lifetime; the constructor acquires it and the destructor releases it. Smart pointers are RAII for heap memory (`#include <memory>`).
- `unique_ptr<T>`: sole ownership. **Cannot be copied**, only moved (`std::move`). Same size as a raw pointer. Create with `make_unique<T>(args)` (C++14). `get()` returns the raw pointer, `release()` gives up ownership (returns the raw pointer without deleting), `reset()` deletes the old object. `unique_ptr<T[]>` for arrays.
- `shared_ptr<T>`: shared ownership through a **reference count** in a control block; `use_count()`. The object is deleted when the last shared_ptr is destroyed. Copying increments the count. `make_shared` allocates the object + control block at once. Size = 2 pointers.
- `weak_ptr<T>`: non-owning, does not increase the count. Use `lock()` to get a shared_ptr (empty if the object is gone), `expired()`. Used to **break reference cycles** (A ↔ B holding shared_ptrs to each other → never freed).
- **Custom deleter**: `unique_ptr<FILE, decltype(&fclose)> f{fopen(...), &fclose};` or a lambda.
- Rules: prefer `unique_ptr`; use `shared_ptr` only for real shared ownership. Never create two smart pointers from the same raw pointer. A function that only uses an object should take `T&` or `T*`, not a smart pointer.

### Exceptions (section 18)
#### Throwing and catching
- `throw` throws an object (of any type: int, string, a class); `try { } catch (const T &e) { }` catches it.
- On a throw, **stack unwinding** happens: functions are exited one by one and every local object's destructor runs, until a matching catch is found. No matching catch → `std::terminate()` → abort.
- catch blocks are tried **in the order written** and the first match wins → put derived classes before base classes. There are no numeric conversions (throw 3.14 does not match `catch (int)`), only Derived → Base. `catch (...)` catches everything and goes last.
- **Throw by value, catch by const reference** (no copy, no slicing, `what()` stays polymorphic).

#### Rethrowing and exception classes
- `throw;` (no expression) rethrows the current exception, keeping its real type. `throw e;` throws a copy (may slice).
- Standard hierarchy (`<stdexcept>`): `std::exception` (`virtual const char *what() const noexcept`) → `logic_error` (`invalid_argument`, `out_of_range`, `length_error`, `domain_error`), `runtime_error` (`overflow_error`, `underflow_error`, `range_error`), `bad_alloc` (new failed), `bad_cast` (dynamic_cast to a reference failed).
- Custom exception classes: derive from `std::exception` (override `what()`) or from `std::runtime_error` (pass the message to the constructor).

#### noexcept, constructors and limits
- `noexcept`: the function promises not to throw. If it throws anyway → `std::terminate`. Destructors are noexcept by default. **Never throw from a destructor.** Move constructors should be noexcept so vector uses them when reallocating.
- An exception in a constructor: the object is considered never created and its destructor does **not** run (but the destructors of fully built members do) → use RAII members.
- Not exceptions: integer division by zero (UB, usually SIGFPE), out-of-bounds access with `[]`, dereferencing nullptr (SIGSEGV).
- Use exceptions for exceptional cases, not for normal control flow.

### I/O and streams (section 19)
- Hierarchy: `ios_base` → `ios` → `istream` / `ostream` → `iostream`; files: `ifstream`, `ofstream`, `fstream` (`<fstream>`); strings: `istringstream`, `ostringstream`, `stringstream` (`<sstream>`).
- Standard objects: `cin` (istream), `cout` (ostream, buffered), `cerr` (unbuffered), `clog` (buffered).
- Manipulators (`<iomanip>`): `setw(n)` (**applies only to the next output**), `setfill(c)`, `left`/`right`/`internal`, `fixed`/`scientific`, `setprecision(n)`, `showpoint`, `showpos`, `boolalpha`, `hex`/`dec`/`oct`, `showbase`, `uppercase`. Except setw, manipulators are **sticky** until changed.
- `setprecision(n)` without fixed: n **significant digits**; with fixed: n digits after the decimal point. The default is 6 significant digits.
- Opening a file: `std::ifstream in{"data.txt"};` or `in.open(...)`. Always check `if (!in)` or `in.is_open()`. Modes: `ios::in`, `ios::out` (overwrite), `ios::app` (append), `ios::trunc`, `ios::binary`, `ios::ate`. The file closes automatically when the object is destroyed (RAII); `close()` closes early.
- Reading: `in >> x` (by word), `getline(in, line)` (by line), `in.get(c)` (by character, including whitespace). Correct loops: `while (getline(in, line)) {...}` or `while (in >> x)`. **Do not** use `while (!in.eof())`: the last iteration runs once too many with stale data.
- Writing: `out << x`, `out.put(c)`. Binary: `write(reinterpret_cast<const char*>(&x), sizeof x)` / `read`.
- Stream state: `good()`, `eof()`, `fail()` (bad format), `bad()` (serious error). Once failed, every later read is skipped until `clear()`; usually followed by `ignore(numeric_limits<streamsize>::max(), '\n')` to drop the bad input.
- `stringstream` is used to split strings, convert string ↔ number and validate input (`if (ss >> n && ss.eof())`).

### STL (section 20)
#### Parts of the STL and iterators
- Three parts: **containers** (hold data), **iterators** (walk containers, like generalized pointers), **algorithms** (`<algorithm>`, `<numeric>`, work on iterator ranges `[first, last)`). Plus functors/lambdas as criteria.
- Templates: `template <typename T> T max(T a, T b)`, class templates `template <typename T> class Item`. Code is generated on use → definitions must live in headers. Non-type parameters: `template <int N>`.
- `begin()` points to the first element, `end()` points **past** the last one (must not be dereferenced). `cbegin`/`cend` (const), `rbegin`/`rend` (reverse).
- Iterator categories: input, output, forward (`forward_list`, `unordered_*`), bidirectional (`list`, `set`, `map`), random access (`vector`, `deque`, `array`, `string`). `std::sort` needs random access → use `l.sort()` for `list`.

#### Containers
- Sequence containers: `array` (fixed, no dynamic allocation), `vector` (fast add/remove at the end), `deque` (fast at both ends, not fully contiguous), `list` (doubly linked, O(1) insert in the middle given an iterator, no `[]`), `forward_list` (singly linked, no `size()`).
- Associative: `set` (unique keys, ordered), `multiset` (duplicates allowed), `map` (key → value, ordered), `multimap`. Unordered (hashing: `unordered_set`, `unordered_map`): O(1) on average, no order.
- `map[key]` inserts a default element when missing; `at(key)` throws `out_of_range`; use `find`/`count` to check. `insert` does not overwrite an existing key and returns `pair<iterator, bool>`.
- Adapters (no iterators): `stack` (LIFO: `push`/`pop`/`top`), `queue` (FIFO: `push`/`pop`/`front`/`back`), `priority_queue` (a heap, **max-heap** by default: `top` is the largest; for a min-heap use `priority_queue<int, vector<int>, greater<int>>`). `pop()` returns nothing.

#### Algorithms and iterator invalidation
- Common algorithms: `find`, `find_if`, `count`, `count_if`, `sort`, `stable_sort`, `binary_search` (needs sorted input), `min_element`, `max_element`, `for_each`, `transform`, `copy`, `replace`, `reverse`, `unique` (only removes **adjacent** duplicates), `remove`/`remove_if` (does not change the size → needs `erase`), `accumulate` (the result type comes from the initial value: 0 → int).
- Iterator invalidation: a vector reallocation invalidates every iterator; erase inside a loop with `it = c.erase(it)`.

### Lambdas (section 21)
- `[capture](params) mutable noexcept -> ret { body }`. The return type is usually deduced.
- Before lambdas: function objects (functors), i.e. classes with `operator()`. A lambda is really an anonymous functor generated by the compiler; each lambda has its own type → store it in `auto` or `std::function<R(Args...)>`.
- **Stateless** (`[]`): no captures, convertible to a function pointer. **Stateful**: has captures.
- `[x]` copies when the lambda is **created** (read-only unless `mutable`); `[&x]` by reference, reads the value when **called**; `[=]` copies every variable used; `[&]` references every variable used; mixes `[=, &total]`, `[&, x]`; `[this]` (captures the this pointer), `[*this]` (C++17); init captures `[y = x + 1]`, `[p = std::move(ptr)]` (C++14).
- `mutable`: lets the lambda modify its own copy; that copy keeps its state between calls, the outside variable is unchanged.
- Generic lambdas (C++14): `[](auto a, auto b) { return a < b; }`.
- Most common use: predicates/comparators for STL algorithms: `sort(v.begin(), v.end(), [](const P &a, const P &b) { return a.age < b.age; });`.
- Danger: capturing by reference and calling the lambda after the variable went out of scope (returning a `[&]` lambda from a function, storing it in a thread) → dangling.

### Enums (section 23)
- **Unscoped** `enum Color { Red, Green = 5, Blue };`: the names leak into the enclosing scope (name clashes between enums → errors) and convert implicitly to int. Values start at 0; an enumerator without a value is the previous one + 1 (Blue = 6).
- **Scoped** `enum class Color { Red, Green };` (C++11): you must write `Color::Red`, there is **no** implicit conversion to int (use `static_cast<int>`), and different enums cannot be compared → type safe.
- Underlying type: `enum class Status : char { ... };` → `sizeof` = 1. The default for enum class is int.
- Works well with `switch` (gcc warns with `-Wswitch` about missing cases).

## Quiz

### Which line compiles? {#cpp-exc-001}
```cpp
auto p1 = std::make_unique<int>(5);
```
- [ ] `auto p2 = p1;`
- [x] `auto p2 = std::move(p1);`
- [ ] `std::unique_ptr<int> p2(p1);`
- [ ] `std::unique_ptr<int> p2 = p1.get();`
> unique_ptr cannot be copied, only moved. After the move, p1 is nullptr. There is no implicit conversion from a raw pointer to unique_ptr.

### What does `use_count()` print? {#cpp-exc-002}
```cpp
auto a = std::make_shared<int>(1);
{
    auto b = a;
    auto c = b;
}
std::cout << a.use_count();
```
- [ ] 3
- [x] 1
- [ ] 0
- [ ] 2
> b and c raise the count to 3, but they are destroyed at the end of the block, so the count is back to 1.

### What is the main purpose of `weak_ptr`? {#cpp-exc-003}
- [ ] It is faster than shared_ptr
- [x] Observing an object managed by shared_ptr without increasing the reference count, to break reference cycles
- [ ] Replacing unique_ptr
- [ ] Managing arrays
> Access the object through `lock()`; if it has been destroyed, `lock()` returns an empty shared_ptr.

### A holds a `shared_ptr<B>` and B holds a `shared_ptr<A>`, pointing at each other. What happens when the outside shared_ptrs are destroyed? {#cpp-exc-004}
- [ ] Both are freed normally
- [x] Neither is freed (a memory leak) because the counts never reach 0
- [ ] A double free
- [ ] Compile error
> Fix: one side holds a `weak_ptr`.

### Why prefer `make_shared<T>()` over `shared_ptr<T>(new T)`? {#cpp-exc-005}
- [ ] Because `new` is banned in C++11
- [x] It allocates the object and the control block in one go (faster, less fragmentation) and is exception-safe
- [ ] Because shared_ptr(new T) does not count references
- [ ] Because make_shared returns a unique_ptr
> A small drawback: while weak_ptrs remain, the object's memory is not returned, since it shares a block with the control block.

### What does RAII mean? {#cpp-exc-006}
- [ ] Every variable must be initialized
- [x] Tying a resource (memory, file, mutex) to an object's lifetime: acquire in the constructor, release in the destructor
- [ ] Only allocate on the stack
- [ ] Only use smart pointers
> Destructors always run when an object goes out of scope, even during exceptions (stack unwinding). Examples: smart pointers, `std::lock_guard`, `std::fstream`, `std::vector`.

### What does this print? {#cpp-exc-007}
```cpp
try { throw 3.14; }
catch (int e)   { cout << "int"; }
catch (float e) { cout << "float"; }
catch (...)     { cout << "other"; }
```
- [ ] int
- [ ] float
- [x] other
- [ ] Compile error
> 3.14 is a double. catch does not apply numeric conversions (double → int, double → float), only Derived → Base.

### Which catch order is CORRECT? {#cpp-exc-008}
- [ ] `catch (const std::exception&)` then `catch (const std::out_of_range&)`
- [x] `catch (const std::out_of_range&)` then `catch (const std::exception&)`
- [ ] Order does not matter; the compiler picks the best match
- [ ] `catch (...)` first
> catch picks the **first** matching handler in the order written. A base class first hides the derived one (gcc warns). `catch (...)` must go last.

### Why catch exceptions by `const T&`? {#cpp-exc-009}
- [ ] The standard requires it
- [x] It avoids copies and slicing (keeps the derived what())
- [ ] So the exception can be modified
- [ ] So the exception is not destroyed
> `catch (std::exception e)` copies only the base part → what() returns std::exception's message.

### How do `throw;` and `throw e;` differ inside a catch block? {#cpp-exc-010}
- [ ] No difference
- [x] `throw;` rethrows the original object (keeping its real type); `throw e;` throws a copy of e's static type (may slice)
- [ ] `throw;` is invalid
- [ ] `throw e;` ends the program
> Calling `throw;` outside a catch (no exception being handled) calls std::terminate.

### An exception is thrown and no catch block handles it. What happens? {#cpp-exc-011}
- [ ] The program ignores it and continues
- [x] `std::terminate()` is called, which aborts by default
- [ ] It returns error code -1
- [ ] Compile error
> Whether the stack is unwound before terminate is implementation-defined.

### What happens when an exception escapes a `noexcept` function? {#cpp-exc-012}
- [ ] It is ignored
- [ ] It propagates to the caller as usual
- [x] `std::terminate()` is called
- [ ] Compile error
> noexcept is a promise. The compiler does not check it at compile time, but at run time it terminates.

### Why should you not throw from a destructor? {#cpp-exc-013}
- [ ] Destructors cannot contain try
- [x] Destructors are noexcept by default, and a second exception during stack unwinding means std::terminate
- [ ] Destructors have no return value
- [ ] The standard forbids it completely
> If a destructor can fail, handle it (log it) inside, or provide a separate `close()` function for users to call.

### A constructor throws halfway through. Which statement is TRUE? {#cpp-exc-014}
- [ ] The object's destructor still runs
- [x] The object's destructor does not run, but the destructors of fully constructed members do
- [ ] The object's memory leaks
- [ ] Constructors must not throw
> That is why members should be RAII types (unique_ptr, vector) rather than raw pointers allocated in the constructor body.

### Is `int x = 10 / 0;` (with the 0 known only at run time) caught by `catch (...)`? {#cpp-exc-015}
- [ ] Yes, it throws std::runtime_error
- [ ] Yes, it throws std::domain_error
- [x] No, it is undefined behavior (on Linux usually the SIGFPE signal)
- [ ] No, because it is a compile error
> C++ does not turn hardware faults into exceptions. Check yourself and throw.

### What does `std::stoi("abc")` throw? {#cpp-exc-016}
- [ ] `std::out_of_range`
- [x] `std::invalid_argument`
- [ ] `std::bad_cast`
- [ ] Nothing, it returns 0
> A number too large for int (`stoi("99999999999")`) throws `std::out_of_range`.

### What does `new` do by default when memory runs out? {#cpp-exc-017}
- [ ] Returns nullptr
- [x] Throws `std::bad_alloc`
- [ ] Calls abort immediately
- [ ] Returns a garbage pointer
> Only `new (std::nothrow) T` returns nullptr. malloc returns NULL.

### What does this print? {#cpp-exc-018}
```cpp
cout << '[' << setw(5) << 42 << "][" << 42 << ']';
```
- [ ] `[   42][   42]`
- [x] `[   42][42]`
- [ ] `[42   ][42]`
- [ ] `[42][42]`
> setw only applies to the next output, right-aligned by default.

### What does `cout << setprecision(3) << 3.14159;` (no fixed) print? {#cpp-exc-019}
- [ ] 3.142
- [x] 3.14
- [ ] 3.141
- [ ] 3.1
> Without fixed, precision means significant digits (3 of them). With `fixed` it means 3 digits after the point: 3.142.

### What does `cout << hex << 255 << ' ' << 16;` print? {#cpp-exc-020}
- [ ] ff 16
- [x] ff 10
- [ ] 0xff 0x10
- [ ] 255 16
> hex is a sticky manipulator, in effect until `dec`. Add `showbase` for the 0x prefix.

### Why is `while (!file.eof()) { file >> x; process(x); }` the wrong way to read? {#cpp-exc-021}
- [ ] eof() does not exist
- [x] eof only becomes true after a read fails, so the last iteration processes the old (or bad) value again
- [ ] It skips the first element
- [ ] It always loops forever
> Correct: `while (file >> x) { process(x); }`, which checks the result of the read itself.

### Which open mode appends to the end without erasing the old content? {#cpp-exc-022}
- [ ] `ios::out`
- [ ] `ios::trunc`
- [x] `ios::app`
- [ ] `ios::in`
> `ofstream` defaults to `ios::out | ios::trunc` (erases the old content).

### Reading "12 abc 34" into three ints with `ss >> a >> b >> c;` gives? {#cpp-exc-023}
- [ ] a = 12, b = 0, c = 34
- [x] a = 12, b = 0, c is not read (keeps its old value), and the stream is in the fail state
- [ ] An exception
- [ ] a = 12, b = 34
> After a failure every later read is skipped until `clear()`. Since C++11 a failed numeric read stores 0.

### Where does a container's `end()` point? {#cpp-exc-024}
- [ ] The last element
- [x] Just past the last element (must not be dereferenced)
- [ ] nullptr
- [ ] The first element
> The range `[begin, end)` is half-open; for an empty container `begin() == end()`.

### What is `v.size()` after this code? {#cpp-exc-025}
```cpp
vector<int> v{1, 2, 3, 2, 2};
remove(v.begin(), v.end(), 2);
```
- [ ] 2
- [x] 5
- [ ] 3
- [ ] 0
> remove only moves the kept elements to the front and returns the "new end"; the size is unchanged. Use `v.erase(remove(...), v.end());`.

### What is `accumulate(v.begin(), v.end(), 0)` for `vector<double> v{0.5, 0.5, 0.5}`? {#cpp-exc-026}
- [ ] 1.5
- [x] 0
- [ ] 1
- [ ] Compile error
> The sum's type comes from the initial value (0 → int): 0 + 0.5 → 0, and so on, giving 0. Use `0.0`.

### `set<int> s{3, 1, 3, 2, 1};` iterated prints? {#cpp-exc-027}
- [ ] 3 1 3 2 1
- [ ] 3 1 2
- [x] 1 2 3
- [ ] 1 1 2 3 3
> set removes duplicates and sorts ascending (red-black tree). multiset keeps duplicates.

### A `priority_queue<int>` gets pushes of 3, 1, 4. What is `top()`? {#cpp-exc-028}
- [ ] 3
- [ ] 1
- [x] 4
- [ ] An error because it is not sorted
> Max-heap by default. Min-heap: `priority_queue<int, vector<int>, greater<int>>`.

### Which container adapter is LIFO? {#cpp-exc-029}
- [x] `stack`
- [ ] `queue`
- [ ] `deque`
- [ ] `priority_queue`
> `queue` is FIFO. `deque` is a real container (with iterators), not an adapter; stack and queue use a deque underneath by default.

### Why can't you call `std::sort` on a `std::list`? {#cpp-exc-030}
- [ ] A list has no elements
- [x] std::sort needs random-access iterators, and list only has bidirectional ones
- [ ] A list is always sorted
- [ ] sort only works on C arrays
> Use the member function `l.sort()` (a merge sort on the linked list).

### What does this print? {#cpp-exc-031}
```cpp
int x = 1;
auto f = [x]() { return x; };
auto g = [&x]() { return x; };
x = 10;
cout << f() << ' ' << g();
```
- [ ] 10 10
- [x] 1 10
- [ ] 1 1
- [ ] 10 1
> A by-value capture is taken when the lambda is created; a by-reference capture reads the value when called.

### What about the lambda `[n]() { return ++n; }` (without mutable)? {#cpp-exc-032}
- [ ] Returns n + 1, and the outer n increases
- [ ] Returns n + 1, the outer n is unchanged
- [x] Compile error: a by-value capture is read-only
- [ ] Undefined behavior
> A lambda's `operator()` is const by default. Add `mutable` to modify its own copy; the outer variable still does not change.

### Which lambda converts to a function pointer `int (*)(int)`? {#cpp-exc-033}
- [x] `[](int a) { return a * 2; }`
- [ ] `[k](int a) { return a * k; }`
- [ ] `[&](int a) { return a; }`
- [ ] `[=](int a) { return a; }` using an outside variable
> Only capture-less (stateless) lambdas convert to function pointers, e.g. to pass to `qsort` or a C API.

### A function returns `[&local]() { return local; }` where `local` is a local variable. What happens when the lambda is called after the function returns? {#cpp-exc-034}
- [ ] It returns the last value of local
- [x] Undefined behavior (a dangling reference)
- [ ] Compile error
- [ ] It returns 0
> When a lambda outlives the current scope, capture by value (`[local]`) or move into it (`[p = std::move(p)]`).

### For `enum Color { Red, Green = 5, Blue };`, what is `Blue`? {#cpp-exc-035}
- [ ] 2
- [x] 6
- [ ] 7
- [ ] 0
> An enumerator without a value is the previous one + 1. Red = 0.

### Which statements about `enum class` compared to a plain enum are TRUE? (select all that apply) {#cpp-exc-036}
- [x] You must qualify the names: `Color::Red`
- [x] No implicit conversion to int; use static_cast
- [x] Two different enum classes can both have an enumerator named Red without clashing
- [ ] It cannot be used in a switch
> enum class works well in a switch with `case Color::Red:`.

### For `enum class Status : char { Ok, Error };`, what is `sizeof(Status)`? {#cpp-exc-037}
- [x] 1
- [ ] 4
- [ ] 8
- [ ] 2
> The underlying type is char. Without one, enum class uses int (4 bytes).
