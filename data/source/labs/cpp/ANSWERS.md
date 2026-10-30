# C++ — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

### 01_ctor_dtor_order — Constructor / destructor order ⚠️ nondeterministic

**Question:** In what order do the ctor/dtor lines appear? Is m1 or m2 created first? What is a?

**Expected output:**

```text
--- create d ---
Base ctor
Member(2) ctor
Member(1) ctor
Derived ctor, b = 10, a = <garbage value>
--- leaving scope ---
Derived dtor
Member(1) dtor
Member(2) dtor
Base dtor
--- end of main ---
```

**Explanation:**

- The base is always built first, then the members in **declaration order in the class**: `m2` then `m1` — writing `m1(1), m2(2)` in the initializer list does not change that.
- Likewise `a` is declared before `b`, so `a(b + 1)` runs while `b` is **not yet initialized** → `a` is garbage (UB).
- Destruction runs backwards: the body of `~Derived` → the members (m1 then m2) → `~Base`.

**Exam relevance:** Exams like a multi-level hierarchy plus member objects and ask for the print order. Rule: "build from the foundation up, demolish from the roof down".

### 02_virtual_slicing — virtual, calls through an object/pointer/reference, and object slicing

**Question:** Does each line print "Animal", "Woof" or "name: ..."? What are sizeof(Animal) and sizeof(Dog)?

**Expected output:**

```text
Animal
Woof
Woof
name: Animal
Animal
Woof
Woof
8 8
```

**Explanation:**

- `Animal a = d` copies only the `Animal` part → `a` really is an Animal (slicing) and calls `Animal::speak`.
- Through a reference/pointer: `speak` is virtual → vtable lookup → `Dog::speak`.
- `name()` is **not virtual** → chosen by the pointer's static type (`Animal*`) → `name: Animal`.
- A class with virtual functions holds one hidden pointer, the **vptr** (8 bytes on x86_64); Dog adds no data, so it is 8 as well.

**Exam relevance:** "Why pass polymorphic objects by `const&`?" → to avoid slicing and copies. Empty class: sizeof = 1; add a virtual function: sizeof = 8.

### 03_virtual_dtor — Virtual and non-virtual destructors

**Question:** In each "---" section, which destructors are called?

**Expected output:**

```text
--- delete through BaseNV* ---
~BaseNV
--- delete through BaseV* ---
~DerivedV
~BaseV
--- object on the stack ---
~DerivedNV
~BaseNV
```

**Explanation:**

- Not virtual: `delete p1` only knows the static type `BaseNV*` → calls `~BaseNV` → DerivedNV's `buf` leaks (formally UB).
- Virtual: the destructor is looked up through the vtable → `~DerivedV`, then `~BaseV` automatically.
- An object on the stack knows its exact type, so it is always destroyed correctly, virtual or not.

**Exam relevance:** Rule: a class with at least one virtual function (used as a polymorphic base) must have a `virtual` destructor. Constructors can **never** be virtual.

### 04_virtual_in_ctor — Calling virtual functions in constructors/destructors

**Question:** Inside the Base ctor and Base dtor, which version of hello() is called?

**Expected output:**

```text
Base ctor -> Base::hello
Derived ctor -> Derived::hello
call() -> Derived::hello
Derived dtor -> Derived::hello
Base dtor -> Base::hello
```

**Explanation:**

- While `Base()` runs, the Derived part (including `msg`) **does not exist yet**; the vptr points to Base's vtable → `Base::hello`. If C++ called down into Derived, the uninitialized `msg` would cause bugs.
- In `~Base()`, the Derived part **has already been destroyed** → `Base::hello` again.
- `call()` is a normal function but calls `hello()` through `this` → normal dynamic dispatch → Derived.

**Exam relevance:** "What happens when you call a virtual function in a constructor?" → the version of the class being built runs, not the derived one. Calling a pure virtual function in a ctor → "pure virtual method called" and abort.

### 05_rule_of_five — Rule of five: copy/move constructors and assignment

**Question:** For the numbered lines 1–7, which call the ctor, copy ctor, copy assignment, move ctor or move assignment? In what order are the dtor lines at the end?

**Expected output:**

```text
ctor(A)
copy ctor
ctor()
copy assign
move ctor
ctor(X)
move assign
dtor(null)
ctor(tmp)
--- end ---
dtor(tmp)
dtor(A)
dtor(X)
dtor(A)
dtor(null)
```

**Explanation:**

- (2) `Str b = a` is **initialization** despite the `=` → copy ctor. (4) `c = a` assigns to an existing object → copy assignment.
- (5) `std::move(a)` turns `a` into an rvalue → move ctor; `a.data` = nullptr.
- (6) `Str("X")` is a temporary (rvalue) → move assignment, then the temporary is destroyed at the end of the statement: `dtor(null)`.
- (7) C++17: a returned prvalue **initializes `e` directly** → a single `ctor(tmp)`, no move/copy.
- Destruction in reverse declaration order: e(tmp), d(A), c(X), b(A), a(null).

**Exam relevance:** Rule of three: if you write one of {dtor, copy ctor, copy assignment}, write all 3. The rule of five adds the move ctor + move assignment. Rule of zero: use RAII (string, vector, unique_ptr) so you write none of them.

### 06_smart_ptr — unique_ptr, shared_ptr, weak_ptr

**Question:** What does use_count print at each point? Are Node A and Node B destroyed? What about Fixed X/Y, and in which order?

**Expected output:**

```text
u1 is empty, *u2 = 42
use_count inside block = 3
use_count after block = 1
Node A ctor
Node B ctor
a.use_count = 2
--- after the Node block ---
x.use_count = 1
Fixed Y dtor
Fixed X dtor
--- end of main ---
```

**Explanation:**

- After the `move`, `u1` is empty (nullptr). Copying a `unique_ptr` is a compile error.
- `s1, s2, s3` share ownership → 3; after the block only 1 remains.
- A holds B and B holds A: when the variables `a` and `b` are destroyed, each Node still has count = 1 → **"Node ... dtor" is never printed** → memory leak.
- With `weak_ptr`, x's count is 1; at the end of the block `y` (declared later) is destroyed first → `Fixed Y dtor`, then `Fixed X dtor`.

**Exam relevance:** Common: `make_shared` allocates the object and the control block in one go. `shared_ptr` is thread-safe for its counter, not for the object. Use `weak_ptr::lock()` to get a temporary `shared_ptr`.

### 07_lambda_capture — Lambda capture by value and by reference

**Question:** What do byVal and byRef print after x changes to 20? What does the third inc() return, and what is the outer counter? What about all() and the last line?

**Expected output:**

```text
byVal = 10, byRef = 20
inc() = 3, counter = 0
all() = 20, counter = 5
0 1 4 
```

**Explanation:**

- `byVal` captured `x = 10` when it was created; `byRef` reads the real variable → 20.
- `inc` has its own copy of `counter` (kept between calls because a lambda is an object) → 1, 2, 3. The outer `counter` is still 0.
- `all` captured `x = 20, counter = 0` when created → 20, even though `allRef` later raises the real counter to 5.
- Each lambda in the for loop captures its own value of `i` → 0 1 4.

**Exam relevance:** A lambda is really a hidden class with a `const` `operator()`; by-value captures are its members. Capture `this` in a member function to access members.

### 08_stl_traps — STL traps: map[], erasing in a loop, iterator invalidation

**Question:** What is the map's size after reading m["b"]? What is left in vector v? What are the capacities before/after, and does p still point to w[0]?

**Expected output:**

```text
size before = 1
m["b"] == 0
size after [] = 2
count("c") = 0, size = 2
1 3 5 | size = 3
capacity = 2
capacity = 4
p == &w[0] ? no
```

**Explanation:**

- Reading `m["b"]` inserts `{"b", 0}` → size 2. To test for existence use `count()` or `find()` (no insertion).
- The correct erase loop uses the iterator returned by `erase`. Writing `v.erase(it); ++it;` uses an invalidated iterator → UB.
- libstdc++ doubles the capacity when full: 2 → 4. The new memory differs from the old, so `p` becomes a **dangling pointer**.

**Exam relevance:** `reserve` changes the capacity, `resize` the size. `map` is ordered by key (red-black tree, O(log n)); `unordered_map` is a hash table (O(1) on average, no order).

### 09_name_hiding — Name hiding and using Base::f

**Question:** What do the calls on d and x print? If you uncomment d.f(1), does it compile?

**Expected output:**

```text
Derived::f(const char*)
Base::f(int)
Base::g(int)
Fixed::f(const char*)
Base::f(int)
Base::f(double)
Base::f(int)

After uncommenting d.f(1):
error: invalid conversion from 'int' to 'const char*'
```

**Explanation:**

- Name lookup stops at the **first** scope that contains an `f` (Derived), and only then considers overloads → `Base::f(int)` is never considered → `d.f(1)` fails.
- `g` is not hidden, because Derived declares no `g`.
- `x.f('A')`: `char` → `int` is a **promotion**, preferred over the conversion to `double` → `Base::f(int)`.

**Exam relevance:** Distinguish **overload** (same scope, different parameters, compile time), **override** (virtual, same signature, run time) and **hiding** (a same-named member in the derived class hides the base's).

### 10_virtual_default_arg — Default arguments of virtual functions, and virtual calls through a normal function

**Question:** Does each line print "Base::f" or "Derived::f", and is x 1 or 2?

**Expected output:**

```text
1: Derived::f x=1
2: Derived::f x=1
3: Derived::f x=2
4: Base::f x=1
5: Derived::f x=1
6: Base::f x=1
7: Derived::f x=5
```

**Explanation:**

- 1, 2: called through `Base*`/`Base&` → Derived's function (dynamic binding), but the default argument comes from the static type `Base` → x = 1.
- 3: called through a `Derived` → the static type is Derived → x = 2.
- 4: `pb->Base::f()` names the class → disables the virtual mechanism and calls Base::f directly.
- 5: inside `run()`, `f()` is `this->f()` with `this` of type `Base*` → Derived::f, x = 1.
- 6: `b` is a real Base object (sliced) → Base::f.
- 7: an explicit argument → no trap.

**Exam relevance:** "Which class's default argument does a virtual function use?" — the static type's. Effective C++: never redefine an inherited virtual function's default argument.

### 11_exception_raii — Exceptions, stack unwinding and RAII

**Question:** In what order do the +/− lines appear? Which catch handles each exception? What does what() print when caught by value? Does ~Bad run?

**Expected output:**

```text
+main
+L1
+L2
-L2
-L1
-main
main caught: error in level2
caught by value: std::exception
int 42
+member
-member
Bad's constructor threw
```

**Explanation:**

- The exception travels up the stack: L2 is destroyed, the `logic_error` catch in level1 does not match (`runtime_error` is not derived from `logic_error`) → L1 is destroyed → main's guard is destroyed **before** entering the catch block.
- "level1 finished normally" is not printed because the exception is not caught in level1.
- Catching `std::exception` by value copies only the base part (slicing) → std::exception's `what()` returns `"std::exception"` (libstdc++).
- catch blocks are tried in the order written, with no type conversions (int does not match double).
- Bad's constructor throws → the object never "lived", so `~Bad` does not run, but the member already built (`member`) is destroyed.

**Exam relevance:** RAII = Resource Acquisition Is Initialization: acquire in the constructor, release in the destructor → no leaks even with exceptions. Never let an exception escape a destructor (destructors are `noexcept` by default).

### 12_diamond — Multiple inheritance, the diamond and virtual inheritance

**Question:** In what order does D d; print its constructors? How many times is A created? What about VD? And the order for E (which lists C before B)?

**Expected output:**

```text
D : A B A C D
d.B::x = 1, d.C::x = 2
VD: A VB VC VD
vd.VB::x = 7, vd.VC::x = 7
E : A C A B E
sizeof: A=4 D=8 VD=24
```

**Explanation:**

- D derives from B then C, each with its own A → A is built twice and D holds 2 copies of `x`.
- With `virtual A` inheritance, VB and VC share one A, and A's constructor is called **first**, by the most derived class VD.
- E's initializer says `B(), C()`, but it is declared `: C, B` → the real order is C then B (gcc warns with -Wreorder).
- sizeof(VD) = 24: the VB part and the VC part each have a vptr (8 + 8) — the vtable stores the offset to the shared A — plus A (4) and padding.

**Exam relevance:** Initialization order: virtual bases → bases in declaration order → members in declaration order → constructor body. Destruction in reverse.

### 13_static_member — static members, static locals and the singleton

**Question:** In what order do the "create/copy/destroy" lines appear? Does make() call copy? How many times is Logger() printed? What is sizeof(Counter)?

**Expected output:**

```text
alive=0 created=0
create #1
copy #1 -> #2
copy #1 -> #3
alive=3 created=3
destroy #3
create #4
alive=3 created=4
destroy #4
destroy #2
destroy #1
alive=0 created=4
Logger()
n = 2, same object? 1
sizeof(Counter) = 4
```

**Explanation:**

- `Counter b = a` → copy #2. `by_value(a)` → copy into the parameter, #3; the parameter is destroyed when the function returns (destroy #3).
- `make()` returns a prvalue → C++17 guarantees copy elision: only "create #4", no copy.
- At the end of the block: destroyed in reverse creation order: c(#4), b(#2), a(#1).
- `static Logger inst` is only initialized on the first call → "Logger()" is printed once; every call returns the same object.
- sizeof only counts non-static members → 4.

**Exam relevance:** A static member function has no `this`, so it cannot access normal members and cannot be virtual/const. The Meyers singleton (a static local) is thread-safe since C++11.

### 14_move_noexcept — std::move, const and noexcept when a vector reallocates

**Question:** When the vector grows its capacity, are the old elements copied or moved for A and for B? What does std::move(ca) call? What is left in s after it is moved from?

**Expected output:**

```text
A, growing capacity: copy copy 
B, growing capacity: move move 
push_back(lvalue): copy | push_back(std::move): move | emplace_back(): 
A x = std::move(ca): copy 
s = [], t = [hello]
```

**Explanation:**

- The 3rd element exceeds capacity 2 → the vector allocates new memory and transfers the 2 old elements. A's move constructor is **not** noexcept → the vector uses `std::move_if_noexcept` and picks the copy. B's is `noexcept` → move.
- The new element created by `emplace_back()` is constructed in place → prints nothing.
- `std::move(ca)` has type `const A&&` → it does not match `A(A&&)` → `A(const A&)` is chosen → copy.
- A moved-from string is "valid but unspecified"; libstdc++ leaves it empty, but you must not rely on that.

**Exam relevance:** Always mark move constructors/move assignment `noexcept`. `std::move` = `static_cast<T&&>`. Do not write `return std::move(local)`, because it prevents NRVO.

### 15_sizeof_class — sizeof of classes: empty classes, the vptr, padding, static members

**Question:** What is sizeof of each struct on x86_64? Do two Empty objects share an address?

**Expected output:**

```text
Empty        = 1
OneInt       = 4
WithFunc     = 4
WithVirtual  = 8
IntVirtual   = 16
DerivedEmpty = 4
TwoEmpty     = 2
CDC          = 24
DCC          = 16
&e1 != &e2 ? 1
```

**Explanation:**

- An empty class is 1 byte so two different objects have different addresses.
- Member functions and static members take no space in the object.
- Virtual functions → an extra 8-byte vptr. `IntVirtual` = vptr 8 + int 4 + padding 4 = 16. Adding more virtual functions does not change sizeof (only the vtable grows).
- Empty Base Optimization: an empty base class costs no bytes → DerivedEmpty = 4. But two empty **members** take 1 byte each → 2.
- `char, double, char` → 1 + 7 pad + 8 + 1 + 7 pad = 24; reordered as `double, char, char` → 16.

**Exam relevance:** "sizeof of an empty class in C++" = 1 (in C, an empty struct is a gcc extension with sizeof 0). vtable: one table per class; vptr: one pointer per object.

### 16_templates — Templates: type deduction, specialization, overloading with a normal function

**Question:** Which version does each maxv call pick: the normal function, the template, or the specialization? What are Box<int>::count and Box<double>::count? Fact<5>::v?

**Expected output:**

```text
non-template 2
template     2.5
template     4
specialized  abd
template     b
non-template 2
Box<int>::count = 2, Box<double>::count = 1
Fact<5>::v = 120
```

**Explanation:**

- `maxv(1, 2)`: the normal function and the template match equally → the normal function wins.
- `maxv(1.5, 2.5)`: only the template matches exactly (T = double).
- `maxv<int>(3, 4)`: explicitly naming the template forces it.
- `maxv("abc", "abd")`: T is deduced as `const char*` → the specialization is used (compares contents with strcmp, not addresses).
- `maxv(1, 2.5)`: the template cannot deduce T (int or double?) → it is discarded; the normal function accepts it by converting 2.5 → 2.
- Box<int> and Box<double> are two different classes → separate statics.
- Fact is computed at compile time: 5! = 120.

**Exam relevance:** Template definitions must live in headers (code is generated where they are used). Full specialization uses `template <>`; class templates can be partially specialized, function templates cannot.

### 17_types_conversion — Variables, fundamental types, integer division and conversions

**Question:** 7/2, 7%2, -7/2, -7%2? What is avg = (1+2+4)/3? What does 'A'+1 print? unsigned 0 minus 1? 10/3*3 and 10*3/3?

**Expected output:**

```text
7/2 = 3, 7%2 = 1, -7/2 = -3, -7%2 = -1
a/2.0 = 3.5, static_cast<double>(a)/b = 3.5, static_cast<double>(a/b) = 3
avg = 2
10/3*3 = 9, 10*3/3 = 10
int t = 3.99 -> 3, (int)-3.99 = -3
c + 1 = 66, char(c + 1) = B, 'a' - 'A' = 32
unsigned 0 - 1 = 4294967295
bool flag = 42 -> 1, boolalpha: true
sizeof: char 1, short 2, int 4, long 8, long long 8, float 4, double 8, bool 1
PI = 3.14159, SIZE = 10, sizeof(arr) = 40
```

**Explanation:**

- Integer division truncates toward zero (C++11 and later): -7/2 = -3, and the sign of % follows the dividend: -7 % 2 = -1.
- `static_cast<double>(a) / b`: cast first → real division 3.5. `static_cast<double>(a / b)`: integer division first (3), cast afterwards → 3.
- `(1 + 2 + 4) / 3` is all int → 7 / 3 = 2, and stored in a double it is still 2.
- `10 / 3 * 3`: left to right, (10 / 3) = 3, × 3 = 9. `10 * 3 / 3` = 30 / 3 = 10.
- Assigning a double to an int with `=` truncates the fraction (toward zero); `int bad{3.99}` is a narrowing error.
- `c + 1`: char is promoted to int → 66 (a number). To print the character, cast back to `char`.
- unsigned overflow is well defined: it wraps modulo 2³² → 4294967295.
- Every non-zero value converted to bool is `true` (printed as 1, or `true` with boolalpha).
- sizeof(long) = 8 on 64-bit Linux (LP64); 4 on 64-bit Windows (LLP64).

**Exam relevance:** The classic "wrong average" question is integer division. `const` may be a run-time constant; `constexpr` must be computable at compile time, so it can be used as an array size.

### 18_operators — Operators: ++/--, compound assignment, comparison, logical operators and short-circuit

**Question:** i, j, k after i++ and ++i? a after the chain of compound assignments? cnt after true || inc()? 0.1 + 0.2 == 0.3? 1 < 2 < 3 and 3 > 2 > 1? 5 / 2 * 2.0?

**Expected output:**

```text
after j = i++: i = 6, j = 5
after k = ++i: i = 7, k = 7
a = 4
m *= 2 + 1 -> 9
(n != 0) && (10 / n > 1) = 0 (no division by 0)
t1 t2 t3 = 101, cnt = 1
true || false && false = 1
!0 + 1 = 2, !(0 + 1) = 0
0.1 + 0.2 == 0.3 ? 0, |diff| < 1e-9 ? 1
1 < 2 < 3 = 1, 3 > 2 > 1 = 0
5 / 2 * 2.0 = 4, 5 / 2.0 * 2 = 5
2 + 3 * 4 = 14, (2 + 3) * 4 = 20, 10 - 4 - 3 = 3
```

**Explanation:**

- `j = i++`: j gets the old value 5, i becomes 6. `k = ++i`: i goes to 7, then k gets 7.
- The compound assignments: 10 + 10 = 20 → 17 → 34 → 34 / 3 = 11 → 11 % 7 = 4.
- `m *= 2 + 1` is `m = m * (2 + 1)` = 9 (compound assignment has the lowest precedence), not `m * 2 + 1`.
- `(n != 0) && ...`: the left side is false, so the right side never runs and there is no division by 0.
- `true || inc()` and `false && inc()` do not call inc(). Only `false || inc()` does → cnt = 1.
- `&&` binds tighter than `||`: `true || (false && false)` = true.
- `!0 + 1`: unary `!` binds tighter than `+` → (!0) + 1 = 2.
- 0.1, 0.2 and 0.3 are not exact in binary → compare with an epsilon.
- `3 > 2 > 1` = `(3 > 2) > 1` = `1 > 1` = false.
- `5 / 2 * 2.0`: integer division first (2), then × 2.0 = 4.

**Exam relevance:** Do not modify a variable twice in one expression without sequencing (e.g. `i = i++ + ++i` is UB). Short-circuiting is the usual way to guard pointers: `if (p && p->x > 0)`.

### 19_control_flow — Control flow: range-for copy/reference, do-while, continue/break, dangling else, ?:

**Question:** What is the vector after for (auto x : v) and for (auto &x : v)? How many times does a do-while run when the condition is false right away? How many loops does a break in nested loops leave? Which if does the else belong to? (true ? 1 : 2.5) / 2?

**Expected output:**

```text
after for (auto x : v):  1 2 3 
after for (auto &x : v): 10 20 30 
do-while runs once
continue/break: 0 2 4 6 
break in nested loops: 00 10 20 
the else belongs to if (y)
score 75: pass
(true ? 1 : 2.5) / 2 = 0.5
weekday
for with 2 variables: sum = 25
```

**Explanation:**

- `auto x` is a copy, so changing x does not change v. `auto &x` refers to the element and changes v directly.
- do-while tests its condition at the end, so the body runs at least once. while tests first, so it may run zero times.
- Odd k → `continue` skips; k = 8 > 6 → `break` → prints 0 2 4 6.
- `break` only leaves the inner loop (over c); the outer loop (over r) still runs 3 times → 00 10 20.
- Misleading indentation: the `else` binds to the nearest `if (y)`. x = 1, y = 0 → the else runs.
- The two branches of `?:` are int and double → the common type is double → 1.0 / 2 = 0.5.
- `for (a = 1, b = 10; a < b; ++a, --b)`: adds 9 + 7 + 5 + 3 + 1 = 25.

**Exam relevance:** A `;` right after `for (...)` or `if (...)` makes an empty body. `switch` only takes integer/enum/char types, not strings or doubles; several values share a case by stacking labels without break.

### 20_strings — C-style strings and std::string

**Question:** sizeof(s) and strlen(s) for char s[] = "Hello"? What does s1 == s2 (char arrays) compare? substr, find, npos? "Zebra" < "apple"? stoi("42abc")? What does cin >> read from "Frank Smith"?

**Expected output:**

```text
sizeof(s1) = 6, strlen(s1) = 5
s1 == s2 ? 0, strcmp(s1, s2) == 0 ? 1
buf = Frank Smith, strlen = 11
string a == b ? 1
c = Hello World, substr(6, 3) = Wor, find("lo") = 3, find("xyz") == npos ? 1
e = abcd, size = 4, length = 4
toupper: ABCD
insert/erase: >>BCD
string(3, '*') = ***
"apple" < "banana" ? 1, "Zebra" < "apple" ? 1
e.at(10) -> out_of_range
stoi("42abc") = 42, to_string(3.5) = 3.500000
isdigit('7') ? 1, isalpha('7') ? 0, isspace(' ') ? 1
in >> first = [Frank], getline rest = [ Smith], line = [Tom Jones]
```

**Explanation:**

- `sizeof` includes the `'\0'` (6); `strlen` counts up to the `'\0'` (5).
- `s1 == s2` on char arrays: both decay to pointers → addresses are compared → false. Use `strcmp`, which returns 0 when equal.
- `strcpy`/`strcat` do not check sizes: buf must be big enough (11 characters + `'\0'`).
- `std::string` compares contents lexicographically by ASCII code. 'Z' = 90 < 'a' = 97, so "Zebra" < "apple".
- `substr(pos, len)`; `find` returns the first position or `string::npos` if not found.
- `size()` and `length()` are identical. `insert(0, ">>")` → ">>ABCD", then `erase(2, 1)` removes the 'A'.
- `at()` checks bounds and throws `out_of_range`; `operator[]` does not (out of bounds is UB).
- `stoi` parses the leading number and stops at the first non-digit; a string that does not start with a number throws `invalid_argument`.
- `>>` skips leading whitespace and stops at whitespace → "Frank". `getline` reads the rest of the line, including the space → " Smith".

**Exam relevance:** The `cin >> n;` then `getline(cin, s);` trap: getline immediately reads the leftover '\n' and returns an empty string. Fix it with `cin.ignore()` or `cin >> ws` before getline.

### 21_functions — Functions: pass by value/reference/pointer, default arguments, overloading, static locals, recursion

**Question:** x after by_val/by_ref/by_ptr? Which overload do print(5.0f), print('a') and print("hi") call? counter() three times? How many calls does fib(5) make?

**Expected output:**

```text
by_val: x = 1
by_ref: x = 200
by_ptr: x = 300
swap_val: 1 2
swap_ref: 2 1
overload: int double double char bool bool string 
Hi Guest 
Hi Frank 
Hi Frank Hi Frank 
counter: 1 2 3
fib(5) = 5, number of calls = 15
factorial(10) = 3628800
```

**Explanation:**

- by_val changes a copy; by_ref and by_ptr change the original. swap_val cannot swap a and b.
- `print(5.0f)`: there is no float overload; float → double is a promotion → the double overload.
- `print('a')`: the char overload is an exact match.
- **`print("hi")` calls the bool overload!** `const char*` → `bool` is a standard conversion, which beats `const char*` → `std::string` (a user-defined conversion, through string's constructor).
- Default arguments are filled from the right; you cannot skip a parameter in the middle.
- `static int c` is initialized only on the first call and keeps its value between calls → 1 2 3.
- Naive recursive fib: the number of calls C(n) = C(n-1) + C(n-2) + 1 → C(5) = 15, growing exponentially.

**Exam relevance:** Overloads differ only by their parameter lists (count, types, const-ness of reference/pointer parameters), never by return type. `print(5L)` is ambiguous because long → int and long → double have the same rank. Every call creates a new stack frame; too-deep recursion overflows the stack (segfault).

### 22_pointers_refs — Pointers and references

**Question:** a and b after r = b and *p = 99? *(q+2), q[3], *q+1, &arr[3]-q? Does reset_val make main's p a nullptr, and reset_ref? sizeof(p) and sizeof(r)?

**Expected output:**

```text
after r = b: a = 20, b = 20, &r == &a ? 1
after r = 30: a = 30, b = 20
after p = &b; *p = 99: a = 30, b = 99
*(q+2) = 30, q[3] = 40, *q+1 = 11, &arr[3]-q = 3
after ++q: *q = 20, q[-1] = 10
reset_val: pa is nullptr ? 0
reset_ref: pa is nullptr ? 1
sizeof(p) = 8, sizeof(r) = 4, sizeof(arr) = 16
*pc = 99, *cp = 1
*one = 5, many = 777
ps->size() = 3, (*pv)[1] = Larry, pv->at(0) = Frank
**pp = 5 -> b = 5
```

**Explanation:**

- `r = b` does not make r refer to b; it assigns b's value (20) to a. r is still a.
- Pointers can be reseated: `p = &b` then `*p = 99` changes b.
- `*q + 1`: `*` binds tighter than `+` → 10 + 1 = 11. `&arr[3] - q` is the number of elements between the two pointers (3), not bytes.
- `q[-1]` is valid because q points into the middle of the array.
- reset_val gets a copy of the pointer → main's pointer does not change. To change it, pass `int*&` (or `int**`).
- `sizeof(r)` is the sizeof of the referred variable (int = 4). `sizeof(p)` is the size of a pointer (8 on 64-bit).
- `const int *pc` lets you change pc but not *pc; `int *const cp` is the opposite. pc points to b = 99, and `*cp = 1` changes a.
- `delete one` after `one = nullptr` does nothing; it is safe.
- `**pp`: pp points to p, which points to b.

**Exam relevance:** References: must be initialized, cannot be null, cannot be reseated. Pointers: can be null, can be reseated, support arithmetic. Common bugs: using a pointer after delete (dangling), deleting twice, forgetting delete (leak), using `delete` on memory from `new[]`.

### 23_class_basics — Classes: constructors, delegating constructors, the copy constructor, const, static, shallow/deep copy

**Question:** Which lines does Player a; print, and in what order? Does display(c) call the copy constructor? What is Player::count at each point? In what order are destructors called when leaving the block? Do the two pointers of the shallow copy share an address?

**Expected output:**

```text
Player a:
  three-args None
  no-args
Player b{"Frank"}:
  three-args Frank
  one-arg
Player c{"Hero", 100, 5}:
  three-args Hero
count = 3
display(c):
  three-args Hero
  copy Hero
  display Hero
  ~Hero
Player d = c:
  three-args Hero
  copy Hero
count = 4, leaving the block:
  ~Clone
  ~Hero
  ~Frank
  ~None
count = 0
  three-args Villain
  one-arg
v.get_name() = Villain
Shallow: same address ? 1, *s1.data = 42
Deep: same address ? 0, *d1.data = 1
end of main:
  ~Villain
```

**Explanation:**

- Delegating constructor: the 3-argument constructor runs completely (body included) before the delegating constructor's body → "three-args" is printed before "no-args".
- `display(Player p)` takes its argument by value → the copy constructor runs; the copy p is destroyed when the function returns (the first `~Hero`).
- `Player d = c;` is initialization, not assignment → the copy constructor.
- Destroyed in reverse creation order: d (renamed Clone), c, b, a.
- The static count is shared by all objects: incremented in the constructor, decremented in the destructor.
- A const object can only call const member functions.
- Shallow: the default copy copies the pointer's value → s1 and s2 point to the same int; a change through s2 shows in s1. If both deleted it, it would be a double free.
- Deep: the hand-written copy constructor allocates new memory.

**Exam relevance:** The compiler only generates a default constructor when the class has no constructors. The copy constructor is called when: initializing from another object, passing by value, returning by value (may be elided). A class owning a resource through a pointer → rule of three/five.

### 24_operator_overloading — Operator overloading

**Question:** After c = a++, what are c and a? How many [pre++] lines does a++ print? Does 2 * a compile, and a * 2? What does (a += b) += b produce?

**Expected output:**

```text
a + b = (4,6), -a = (-1,-2)
a == Vec{1,2} ? 1, a != b ? 1
[pre++] [post++] 
c = a++: c = (1,2), a = (2,3)
[pre++] 
d = ++a: d = (3,4), a = (3,4)
2 * a = (6,8)
a[1] = 9 -> a = (3,9)
(e += b) += b -> e = (7,9)
Vec f = 5 -> (5,0), f + 1 = (6,0)
```

**Explanation:**

- Postfix `operator++(int)`: saves the old copy, calls prefix (prints `[pre++]`), then returns the old copy → c = (1,2), a = (2,3).
- Prefix returns `*this` by reference, without making a copy.
- `2 * a`: the left operand is an int, so it must be a non-member function (a friend here). `a * 2` is an error because it is not defined.
- `operator[]` returns `int&`, so `a[1] = 9` works.
- `+=` returns `Vec&` → `(e += b) += b` adds b twice to the same e: (1,1) + (3,4) + (3,4) = (7,9).
- A non-`explicit` constructor allows implicit conversion: `Vec f = 5` and `f + 1` (1 → Vec(1,0)).

**Exam relevance:** You cannot overload `.` `.*` `::` `?:` `sizeof`. These must be members: `=`, `[]`, `()`, `->`. Stream `<<` and `>>` must be non-members. Overloading cannot change precedence or the number of operands.

### 25_inheritance — Inheritance: base constructors, redefinition and virtual functions

**Question:** In what order are the constructors printed when creating a Trust? Through an Account&, which versions of withdraw (not virtual) and print (virtual) run? What does the loop over vector<Account*> print? In what order are destructors called on delete?

**Expected output:**

```text
Create Trust t:
  Account(Tom)
  Savings(Tom)
  Trust(Tom)
ref.withdraw(100):
  Account::withdraw
t.withdraw(100):
  Savings::withdraw
  Account::withdraw
ref.print():
  [Trust Tom: 800]
Create vector<Account*>:
  Account(A)
  Account(S)
  Savings(S)
Print through Account*:
  [Account A: 10]
  [Savings S: 20, 3%]
delete:
  ~Account(A)
  ~Savings(S)
  ~Account(S)
sh.area() = 9
end of main:
  ~Trust(Tom)
  ~Savings(Tom)
  ~Account(Tom)
```

**Explanation:**

- Constructors go from the root base down: Account → Savings → Trust. Destructors go the other way.
- `withdraw` is not virtual → static binding: through an `Account&` it calls `Account::withdraw`; through the Trust object it calls `Savings::withdraw` (the closest one up the hierarchy).
- `print` is virtual → dynamic binding: even through an `Account&` it calls `Trust::print`. The balance is 800 after two withdrawals of 100.
- `~Account` is virtual, so `delete` through an `Account*` calls `~Savings` and then `~Account`.
- A class with a pure virtual function (`= 0`) is abstract: you cannot create objects of it, but you can use references/pointers to it.

**Exam relevance:** `public` inheritance keeps access as is, `protected` turns public into protected, `private` turns everything private; a base's private members are never accessible from the derived class. A derived class does not inherit constructors, the destructor or operator= (unless `using Base::Base`). `override` makes the compiler report signature mismatches.

### 26_exceptions — Exceptions: catch order, catch(...), rethrow, custom exception classes

**Question:** Which catch block handles each test(k) call? For which k is the "after test(k)" line printed? When catching by value (catch(std::exception e)), what does what() return? What do stoi("abc") and v.at(10) throw?

**Expected output:**

```text
  catch int 42
  catch string: an error as a string
  catch InsufficientFunds: Insufficient funds, need = 50
  catch exception: bad argument
  catch (...)
  k = 5: no throw
  after test(5)
  rethrow(): caught "from rethrow", rethrowing
  main caught it again: from rethrow
  caught by value: what() = std::exception
  stoi("abc") -> invalid_argument
  v.at(10) -> out_of_range
```

**Explanation:**

- Anything can be thrown: int, string, double, a class. catch blocks are tried in order and there is no numeric conversion (3.14 does not match `catch (int)`) → it falls to `catch (...)`.
- `invalid_argument` derives from `logic_error` → `exception` → caught by `catch (const exception&)`.
- InsufficientFunds is also an `exception`, but `catch (const InsufficientFunds&)` comes first, so it is chosen.
- After a `throw`, the rest of the try block is skipped → "after test(k)" is only printed for k = 5.
- `throw;` rethrows the original object (keeping its dynamic type) to the enclosing try block.
- `catch (exception e)` copies the base part → slicing → std::exception's `what()` ("std::exception").

**Exam relevance:** Throw by value, catch by const reference. Never throw from a destructor (during stack unwinding it calls `std::terminate`). An uncaught exception → `std::terminate` → abort.

### 27_streams — I/O streams: iomanip, sticky/non-sticky manipulators, stringstream

**Question:** What does 1234.5678 print by default? setprecision(2) with and without fixed? How many outputs does setw apply to? Does hex stay in effect for the next output? What happens when "12 abc" is read into two ints?

**Expected output:**

```text
default:             1234.57, 1234567.89 -> 1.23457e+06
setprecision(2):     1.2e+03
fixed setprecision(2): 1234.57
still fixed:         3.14
[    42][42]
[42    ][ab    ]
[****42][**7]
ff 10 0xff 255
10 false 2.00000 2
read: Frank | 100 | 1234.5
"12 abc 34": x = 12, y = 0, z = 0, fail = 1
after clear(): skip = abc, z = 34
ostringstream: [Total:    42!]
123abc is not an integer (read 123)
```

**Explanation:**

- By default 6 significant digits are printed: 1234.5678 → 1234.57; large numbers switch to scientific notation, 1.23457e+06.
- `setprecision(2)` without fixed: 2 significant digits → 1.2e+03. With `fixed`: 2 digits after the point → 1234.57.
- `fixed` and the precision stay in effect for later outputs (3.14).
- `setw` only applies to the next output; right alignment is the default, and `left` is sticky. `setfill` is sticky too.
- `hex` stays in effect (16 → 10) until `dec`; `showbase` adds the 0x prefix. `oct` 8 → 10. `showpoint` prints all 6 significant digits: 2.00000.
- Reading "abc" into an int fails: y is set to 0 (C++11) and the stream enters the fail state, so z is never read. Only after `clear()` can reading continue.
- "123abc": 123 is read but the end of the string is not reached (eof is false) → not a valid integer.

**Exam relevance:** Hierarchy: `ios` → `istream`/`ostream` → `iostream`; files use `ifstream`/`ofstream`/`fstream`; strings use `istringstream`/`ostringstream`/`stringstream`. Always check `is_open()` after opening a file. Read line by line with `while (getline(file, line))`; never `while (!file.eof())`, which runs one iteration too many.

### 28_stl_algorithms — STL: iterators, algorithms, container adapters

**Question:** What is accumulate(vd, 0) for vector<double>{1.5, 2.5}? After remove(…, 3), what is the size? What does a set built from {5,1,3,1,5} print? In what order does a map print? priority_queue.top()? Which element do stack/queue give first?

**Expected output:**

```text
accumulate(vd, 0) = 3, accumulate(vd, 0.0) = 4
find 5 at index 4, count 3 = 3, count_if even = 1
max = 5, find 9 == end ? 1
after remove(3): size = 6, kept elements = 3
after erase: 4 1 5 
sort descending: 5 4 1 
transform x*x: 25 16 1 
reverse: 1 16 25 
set: 1 3 5 
insert 3 again: inserted = 0, *pos = 3
map: Frank=11 Larry=3 Moe=2 , size = 3, m.count("Joe") = 0
deque: 1 2 3 4 
list: 1 2 3 4 
stack.top = 5, queue.front = 3, queue.back = 5, pq.top = 5
pq pops: 5 4 3 1 1 
```

**Explanation:**

- `accumulate(..., 0)`: the sum has type int → 0 + 1.5 → 1, 1 + 2.5 → 3. With 0.0 it is correctly 4.
- `find` returns an iterator; `end()` when not found. `it - v.begin()` is the index.
- `remove` shifts the elements other than 3 to the front and returns the "new end"; the size is still 6. You must `erase(new_end, end())` (the erase-remove idiom).
- `sort` with the lambda `a > b` → descending. `transform` can write over its own source container.
- A set removes duplicates and sorts itself; inserting an existing element returns `{iterator, false}`.
- A map is sorted by key (strings in lexicographic order); `m["Moe"]` inserts a new key; `count` only returns 0 or 1.
- stack: LIFO (top = the last pushed, 5). queue: FIFO (front = 3, back = 5). priority_queue: a max-heap.

**Exam relevance:** The three parts of the STL: containers, iterators, algorithms (plus functors/lambdas). Iterator categories: input, output, forward (`forward_list`), bidirectional (`list`, `map`, `set`), random access (`vector`, `deque`, `array`). `std::sort` needs random access, so `list` uses `list::sort()`.

### 29_lambda_enum — Lambdas and enum / enum class

**Question:** After x = 100, what do by_val() and by_ref() print? A mutable counter() called 3 times: does the outer n change? What are Color::Blue and Status::Error? Red == 0 with a plain enum and with enum class?

**Expected output:**

```text
by_val() = 1, by_ref() = 100
counter mutable: 1 2 3, outer n = 0
copy() = 4, counter() = 4
total = 21, elements > 3 = 3
add5(10) = 15, square(3) = 9, square(1.5) = 2.25, IIFE = 42
Red = 0, Green = 5, Blue = 6, Red == 0 ? 1
int as_int = Blue -> 6
Status::Error = E, sizeof(Status) = 1, Color2::Blue = 2
Error
```

**Explanation:**

- `[x]` captures the value 1 when the lambda is created; `[&x]` reads x when called → 100.
- `mutable` allows modifying the lambda's own copy of n, which keeps its state between calls. The outer n is still 0.
- `auto copy = counter;` copies the state too (n = 3) → the two lambdas count independently: both return 4.
- `[&total]` accumulates into the outer variable; `[threshold]` only reads.
- `make_adder` returns a lambda capturing n by value → safe after the function returns. `[](auto a)` is a generic lambda (C++14). A lambda can be called right where it is defined (IIFE).
- enum: values start at 0, and an enumerator without a value is the previous one + 1 → Blue = 6. A plain enum converts to int implicitly.
- `enum class Status : char` sets the underlying type to char → sizeof = 1. enum class does not convert to int implicitly; use `static_cast`.

**Exam relevance:** Syntax: `[capture](params) mutable noexcept -> ret { body }`. A capture-less lambda converts to a function pointer. enum class avoids name clashes (two enums both having `Red`) and accidental comparisons between different enums.
