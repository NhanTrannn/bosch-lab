---
tags: [cpp, stl, smart-pointers, move-semantics, lambdas]
---

# STL and modern C++ (11/14/17)

## Theory

### Containers and complexity
| Container | Structure | Access | Insert/erase | Notes |
| --- | --- | --- | --- | --- |
| `vector` | contiguous dynamic array | O(1) | end: amortized O(1), middle: O(n) | capacity usually grows ×2 (libstdc++) or ×1.5 (MSVC) |
| `deque` | chunks | O(1) | both ends O(1) | |
| `list` | doubly linked list | O(n) | O(1) given an iterator | iterators stay valid on insert |
| `map` / `set` | red-black tree | O(log n) | O(log n) | ordered by key |
| `unordered_map` / `unordered_set` | hash table | O(1) average, O(n) worst | O(1) average | unordered, rehashes when full |
| `array` | fixed-size array | O(1) | — | size known at compile time |
| `stack`, `queue`, `priority_queue` | adapters | | | priority_queue is a heap, max-heap by default |

- `map[key]` **inserts** a default element if the key is missing; use `find`/`count`/`contains` (C++20) to check.
- `size()` is the number of elements, `capacity()` the allocated space. `reserve(n)` only grows capacity; `resize(n)` changes the size.
- Iterator invalidation: when a vector reallocates, **all** iterators/pointers/references become invalid; erasing in the middle of a vector invalidates iterators from that position on. Erasing while looping: `it = v.erase(it);`.
- Erase-remove idiom: `v.erase(remove(v.begin(), v.end(), x), v.end());` (C++20: `std::erase(v, x)`).

### Smart pointers (`<memory>`)
- `unique_ptr<T>`: exclusive ownership; cannot be copied, only moved. Same cost as a raw pointer. Create with `make_unique` (C++14).
- `shared_ptr<T>`: shared ownership via an (atomic) reference count in a control block; the object is destroyed when the count reaches 0. `make_shared` allocates the object and the control block in one go.
- `weak_ptr<T>`: non-owning reference that does not increase the count; use `lock()` to get a shared_ptr. Used to **break reference cycles** (A holds a shared_ptr to B and B to A → never freed).
- Never create two shared_ptrs from the same raw pointer (two control blocks → double delete).

### Move semantics
- lvalue: has a name/address. rvalue: a temporary. `T&&` is an rvalue reference.
- `std::move(x)` is just `static_cast<T&&>(x)`; it moves nothing by itself. A moved-from object is valid but in an unspecified state.
- vector only moves elements when reallocating if the move constructor is `noexcept`.
- Perfect forwarding: `template<class T> void f(T&& x) { g(std::forward<T>(x)); }` (forwarding reference).
- Copy elision: guaranteed in C++17 when returning a prvalue; NRVO for named variables is not guaranteed but compilers usually do it.

### Lambdas
`[capture](params) mutable -> ret { body }`
- `[=]` copies every variable used (value captured when the lambda is **created**), `[&]` by reference, `[x, &y]`, `[this]`, `[*this]` (C++17), init-capture `[p = std::move(ptr)]` (C++14).
- By-value captures are `const` inside the lambda; add `mutable` to modify the copy.
- Capturing by reference and using the lambda after the variable went out of scope → dangling.

### Other features
- `auto`, range-for, `nullptr` (type `nullptr_t`, replaces NULL), `enum class` (scoped, no implicit conversion to int), `constexpr` (evaluated at compile time), `static_assert`, `using` aliases, `= default/= delete`, `override/final`, uniform initialization `{}` (blocks narrowing), structured bindings `auto [a, b] = pair;` (C++17), `std::optional`, `std::variant`, `std::string_view` (C++17).
- Templates: definitions must be in the header. A regular function beats a template when both match equally well.

## Quiz

### What is the average lookup complexity of `std::map` and `std::unordered_map`? {#cpp-stl-001}
- [ ] O(1) and O(1)
- [x] O(log n) and O(1)
- [ ] O(log n) and O(log n)
- [ ] O(n) and O(1)
> map is a balanced red-black tree; unordered_map is a hash table (worst case O(n) with many collisions).

### What does this print? {#cpp-stl-002}
```cpp
std::map<std::string, int> m;
if (m["x"] == 0) {}
std::cout << m.size();
```
- [ ] 0
- [x] 1
- [ ] Throws an exception
- [ ] Undefined behavior
> operator[] inserts a default element when the key is missing. Use `m.find("x")` or `m.count("x")` to only check. `m.at("x")` throws `std::out_of_range`.

### After a `push_back` makes a vector reallocate, what happens to old iterators/pointers to its elements? {#cpp-stl-003}
- [ ] They stay valid
- [x] They are invalidated (using them is UB)
- [ ] They are updated to the new memory automatically
- [ ] Only end() is invalidated
> When capacity grows, the elements move to new memory and the old block is freed.

### How do you erase even elements from a vector while iterating with an iterator? {#cpp-stl-004}
- [ ] `for (auto it = v.begin(); it != v.end(); ++it) if (*it % 2 == 0) v.erase(it);`
- [x] `for (auto it = v.begin(); it != v.end();) if (*it % 2 == 0) it = v.erase(it); else ++it;`
- [ ] `for (int x : v) if (x % 2 == 0) v.erase(x);`
- [ ] You cannot erase while iterating
> erase returns an iterator to the next element; the old iterator has been invalidated.

### After `v.reserve(100)` on an empty vector, what are `size()` and `capacity()`? {#cpp-stl-005}
- [ ] 100 and 100
- [x] 0 and at least 100
- [ ] 100 and 0
- [ ] 0 and 0
> reserve only allocates space, it creates no elements. resize changes the size.

### Which statement about `std::unique_ptr` is true? {#cpp-stl-006}
- [ ] It can be copied, and then both copies own the object
- [x] It cannot be copied; ownership is transferred only by moving
- [ ] It has a reference count
- [ ] It does not free memory automatically
> The copy constructor is deleted. `auto q = std::move(p);` → p becomes nullptr.

### What does this print? {#cpp-stl-007}
```cpp
auto a = std::make_shared<int>(5);
auto b = a;
std::weak_ptr<int> w = a;
std::cout << a.use_count();
```
- [ ] 1
- [x] 2
- [ ] 3
- [ ] 0
> weak_ptr does not increase use_count.

### Which problem does `weak_ptr` solve? {#cpp-stl-008}
- [ ] Leaks from forgetting delete
- [x] Reference cycles between shared_ptrs that keep objects alive forever
- [ ] Double free when copying a unique_ptr
- [ ] Concurrent access from many threads
> E.g. a tree where children hold a shared_ptr to the parent and the parent holds shared_ptrs to the children → one direction must be weak_ptr.

### What does `std::move(x)` actually do? {#cpp-stl-009}
- [ ] Copies x to new memory and deletes x
- [x] Casts x to an rvalue reference so the move constructor/assignment can be selected
- [ ] Frees x's memory
- [ ] Changes x's address
> The actual moving is done by the type's move constructor/assignment.

### What does this print? {#cpp-stl-010}
```cpp
int x = 1;
auto f = [x]() { return x; };
auto g = [&x]() { return x; };
x = 10;
std::cout << f() << " " << g();
```
- [ ] 10 10
- [x] 1 10
- [ ] 1 1
- [ ] 10 1
> A by-value capture is taken when the lambda is created; a by-reference capture reads the current value.

### What do you need to modify a by-value capture inside a lambda? {#cpp-stl-011}
- [ ] `const`
- [x] `mutable`
- [ ] `volatile`
- [ ] `static`
> A lambda's operator() is const by default. mutable only changes the copy inside the lambda, not the outside variable.

### What is the advantage of `nullptr` over `NULL`? {#cpp-stl-012}
- [ ] It is faster
- [x] It has its own type (nullptr_t), so overloads cannot mistake it for an integer
- [ ] It can be dereferenced safely
- [ ] It frees memory automatically
> With `f(int)` and `f(char*)`, `f(NULL)` may call f(int); `f(nullptr)` always calls the pointer overload.

### Main differences of `enum class` compared to a plain `enum`? (select all that apply) {#cpp-stl-013}
- [x] The enumerators are scoped to the enum (`Color::Red`)
- [x] No implicit conversion to int
- [ ] You cannot specify the underlying type
- [ ] It can only be used inside a class
> You can specify the underlying type: `enum class Color : uint8_t { ... }`.

### Which container fits best when you often insert/erase in the middle and need iterators to stay valid? {#cpp-stl-014}
- [ ] vector
- [ ] deque
- [x] list
- [ ] array
> Inserting/erasing in a list does not invalidate iterators to other elements. In practice vector is still often faster for small data thanks to the cache.

### Which element does a default `std::priority_queue<int>` give you first? {#cpp-stl-015}
- [x] The largest
- [ ] The smallest
- [ ] The first inserted
- [ ] The last inserted
> Max-heap. For a min-heap: `priority_queue<int, vector<int>, greater<int>>`.

### What does `constexpr` mean? {#cpp-stl-016}
- [ ] The variable cannot change at run time, like const
- [x] The value/function can be (or, for variables, must be) evaluated at compile time
- [ ] The function is forced inline
- [ ] The variable is placed in ROM
> A constexpr variable must be a compile-time constant; a constexpr function is evaluated at compile time when its arguments are constants.

### Why prefer `make_shared` over `shared_ptr<T>(new T)`? {#cpp-stl-017}
- [ ] Because `new T` is not allowed with shared_ptr
- [x] It allocates the object and the control block at once, which is faster and exception-safe
- [ ] Because make_shared does not count references
- [ ] Because make_shared creates a unique_ptr
> A small drawback: the object's memory is only released once all weak_ptrs are gone too.

### `auto [k, v] = *m.begin();` (m is a std::map) uses which feature? {#cpp-stl-018}
- [ ] Lambda capture
- [x] Structured bindings (C++17)
- [ ] Initializer lists
- [ ] Template deduction
> Unpacks a pair/tuple/struct into named variables.
