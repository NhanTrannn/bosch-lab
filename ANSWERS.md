# ANSWERS — answers and explanations

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

## 01-c

### 01_sizeof_padding — sizeof and struct padding

**Question:** What are sizeof(struct A), sizeof(struct B) and sizeof(struct C)? What is the offset of each member?

**Expected output:**

```text
sizeof(A) = 12 | c1=0 i=4 c2=8
sizeof(B) = 8 | i=0 c1=4 c2=5
sizeof(C) = 6 | c1=0 i=1 c2=5
```

**Explanation:**

- `struct A`: `c1` at 0, 3 bytes of padding so `i` starts at 4 (alignment 4), `c2` at 8, then 3 bytes of **tail padding** so the total is a multiple of 4 → 12.
- `struct B`: `i` at 0, the two `char`s side by side at 4 and 5, 2 bytes of tail padding → 8.
- `#pragma pack(1)` removes all padding → 1 + 4 + 1 = 6, but `i` is misaligned (offset 1): slower to access, and on some architectures (older ARM) it can cause a bus error.

**Exam relevance:** A "sizeof of struct {char; int; char;}" question is almost certain. Tip: order members from largest to smallest to reduce padding. An empty struct in C++ has sizeof = 1.

### 02_pointer_array — Pointer arithmetic, arrays and decay

**Question:** How do sizeof(arr) in main and sizeof(a) in f differ? What does *p++ print? How many bytes apart are pb + 1 and pb?

**Expected output:**

```text
in main: sizeof(arr) = 20
in f   : sizeof(a)   = 8
*(p + 2) = 30
p[3] = 40, 3[arr] = 40
*p++ = 10, then *p = 20
&arr[4] - &arr[0] = 4
sizeof(pa) = 24, sizeof(pb) = 8
*pa[1] = 8, pb[1][2] = 6
(char *)(pb + 1) - (char *)pb = 12
```

**Explanation:**

- In `main`, `arr` is an array of 5 ints → 20 bytes. The parameter `int a[10]` is really `int *a` → 8 bytes on x86_64 (the 10 is ignored).
- `a[i]` is exactly `*(a + i)`, so `3[arr]` is valid and equals `arr[3]`.
- `*p++` = `*(p++)`: takes the old value (10), then advances the pointer.
- The difference of two pointers counts **elements**, not bytes → 4.
- `pa` is an array of 3 pointers → 3 × 8 = 24. `pb` is a single pointer → 8, but `pb + 1` skips a whole `int[3]` row = 12 bytes.

**Exam relevance:** Read declarations with the "right-left from the name" rule: `int *p[3]` → p is an array of 3 elements, each an `int*`. `int (*p)[3]` → p is a pointer to an array of 3 ints.

### 03_const_pointer — The three kinds of const with pointers

**Question:** What does the program print? Uncomment the commented-out lines in main one at a time: which compile, which fail, and with what error?

**Expected output:**

```text
a = 10, b = 2, *p1 = 2, *p2 = 10, *p3 = 10

Uncommenting each line:
*p1 = 10;  -> error: assignment of read-only location '*p1'
p2 = &b;   -> error: assignment of read-only variable 'p2'
*p3 = 20;  -> error: assignment of read-only location '*p3'
p3 = &b;   -> error: assignment of read-only variable 'p3'
```

**Explanation:**

| Declaration | Change the pointer? | Change the value through it? |
|---|---|---|
| `const int *p` (= `int const *p`) | Yes | No |
| `int *const p` | No | Yes |
| `const int *const p` | No | No |

`p1 = &b` is legal, so `*p1` = 2. `*p2 = 10` changes `a` to 10, so `*p3` also reads 10: `const` only forbids modification **through that pointer**; it does not make `a` constant.

**Exam relevance:** The classic: "Which declaration lets you change the pointer but not the value?" → `const int *p`. Remember that `int const *` and `const int *` are the same.

### 04_static_extern — static locals, file-scope static and extern

**Question:** What do n and m print after 3 calls to counter()? If you uncomment the extern int g_hidden line and use it, does it fail at compile time or link time?

**Expected output:**

```text
n = 1, m = 1
n = 2, m = 1
n = 3, m = 1
file_counter = 3
g_shared = 101, hidden = 7

After uncommenting extern int g_hidden and using it:
compiles OK, but LINKING fails: undefined reference to 'g_hidden'
```

**Explanation:**

- `static int n` lives in data/bss, is initialized once and keeps its value between calls. `m` is a stack variable, back to 0 on every call.
- `extern` is only a **declaration**: the compiler trusts that the variable exists somewhere, so it compiles. Only at **link** time does it turn out that `g_hidden` is `static` (no global symbol is exported) → `undefined reference`.
- `static` on a function means the same: the function can only be used in that file.

**Exam relevance:** Distinguish **scope** (visibility) from **lifetime**. `static` has 2 meanings: inside a function → changes the lifetime; outside → changes the linkage. Uninitialized globals default to 0.

### 05_bitwise — Bit manipulation

**Question:** Write down the first 4 binary lines and the values on the last 4 lines.

**Expected output:**

```text
00101100
00101101
00100100
10101100
bit 2 = 1, bit 4 = 0
set bits = 3
is_pow2: 0=0 1=1 6=0 64=1
~x & 0xFF = 0xD3, x >> 2 = 11, x << 1 = 88
```

**Explanation:**

- `0x2C` = 44 = `0010 1100`: bits 2, 3 and 5 are set.
- Each `x &= x - 1` clears exactly one set bit (Kernighan's algorithm) → number of iterations = number of set bits = 3.
- A number is a power of two when exactly 1 bit is set → `x & (x-1) == 0`; x = 0 must be excluded.
- Shifting right by 2 = dividing by 4 (44/4 = 11), shifting left by 1 = multiplying by 2 (88).

**Exam relevance:** Common: "which expression tests for a power of two", "what does x & (x-1) do", "x & -x" (keeps the lowest set bit), "XOR swap". Shifting a negative number left, or by ≥ the type's width, is UB.

### 06_endian_union — Endianness with a pointer and a union

**Question:** On an x86_64 PC, what is the first byte of 0x12345678 in memory? In what order does the "Way 2" line print the 4 bytes?

**Expected output:**

```text
Way 1: first byte = 0x78 -> little endian
Way 2: 78 56 34 12 
Way 3: t.c = 1
sizeof(union U) = 4
```

**Explanation:**

- x86/x86_64 and most ARM (by default) are **little endian**: the byte `0x78` (least significant) is at the lowest address.
- Network byte order is **big endian** → use `htonl/htons/ntohl/ntohs` when sending over a network.
- `sizeof(union)` = the size of the largest member (plus padding, if any) → 4.

**Exam relevance:** "What does this code print on a little-endian machine" with `char *p = (char*)&i` is a classic. Remember: big endian looks like the number written on paper.

### 07_signed_unsigned — signed/unsigned traps and integer division

**Question:** Which branch does line A print? What are the values B, C, D, E, F, G, H?

**Expected output:**

```text
A: -1 >= 1U
B: u - 5 = 4294967294
C: i > k ? 1
D: 7/2=3  -7/2=-3  -7%2=-1  7%-2=1  7/2.0=3.5
E: sizeof("abc")=4 sizeof(s)=4 strlen(s)=3
F: c = 0
G: (unsigned short)sh = 65535
H: int=4 long=8 ptr=8 char=1
```

**Explanation:**

- A, C: `-1` converted to `unsigned` becomes `4294967295` (UINT_MAX), which is greater than 1.
- B: unsigned values cannot be negative; the subtraction wraps around modulo 2^32.
- D: since C99, integer division **truncates toward zero**; the sign of `%` follows the **dividend**.
- E: the string `"abc"` has an extra `'\0'` → 4 bytes; `strlen` does not count `'\0'`.
- F: unsigned overflow is well defined (wraps to 0). Only **signed** overflow is UB.
- H: Linux x86_64 uses the **LP64** model: long and pointers are 8 bytes (64-bit Windows is LLP64: long is 4 bytes).

**Exam relevance:** Very common: `if (-1 < sizeof(int))` → false, because `sizeof` returns `size_t` (unsigned). Whether `char` is signed is implementation-defined (x86: signed, ARM: unsigned).

### 08_macro_traps — Macro traps

**Question:** What is the value on each line? After SQ_OK(i++) and MAX(j++, 3), what are i and j?

**Expected output:**

```text
SQ(a + 1)     = 7
SQ_OK(a + 1)  = 16
10 / SQ(2)    = 10
10 / SQ_OK(2) = 2
DOUBLE(5) * 2 = 15
SQ_OK(i++) = 30, i = 7      <- UB: gcc usually gives 30, other compilers may differ
MAX(j++, 3) = 6, j = 7
```

**Explanation:**

- `SQ(a + 1)` → `a + 1 * a + 1` = 3 + 3 + 1 = 7.
- `10 / SQ(2)` → `10 / 2 * 2` = 10 (divide, then multiply, left to right).
- `DOUBLE(5) * 2` → `5 + 5 * 2` = 15.
- `SQ_OK(i++)` → `((i++) * (i++))`: modifies `i` twice without a sequence point → **undefined behavior**. There is no "correct" answer.
- `MAX(j++, 3)` → `((j++) > (3) ? (j++) : (3))`: `j++` runs twice → returns 6, j = 7.

**Exam relevance:** Exams like `#define SQUARE(x) x*x` with `SQUARE(2+3)` → 11. Wrap multi-statement macros in `do { ... } while (0)`. The `#` operator stringizes a parameter, `##` pastes tokens.

### 09_memory_layout — Memory layout: text, data, bss, heap, stack ⚠️ nondeterministic

**Question:** Which region does each variable live in (text / rodata / data / bss / heap / stack)? Order the addresses from low to high. What are g_uninit and s_global?

**Expected output:**

```text
(addresses change on every run because of ASLR, but the ORDER is fixed, e.g.)
main      : 0x55e1780e71a9   <- text
"hello"   : 0x55e1780e8008   <- rodata
g_const   : 0x55e1780e8004   <- rodata
g_init    : 0x55e1780ea010   <- data
s_local   : 0x55e1780ea014   <- data
g_uninit  : 0x55e1780ea020   <- bss
s_global  : 0x55e1780ea01c   <- bss
heap      : 0x55e1b612c2a0   <- heap
local     : 0x7ffe4fe91ba4   <- stack (0x7ff...)
g_uninit = 0, s_global = 0
(the order of variables WITHIN one region is up to the compiler/linker)
```

**Explanation:**

Addresses increase: **text < rodata < data < bss < heap ... (a big gap, mmap'ed libraries) ... stack**.

- The string constant `"hello"` and the global `const` variable live in `.rodata` (read-only) → modifying `str[0]` causes a **Segmentation fault**.
- `.bss` takes no space in the executable; the loader fills it with zeros.
- The heap grows up (increasing addresses), the stack grows down (decreasing addresses).
- The `size` command prints the sizes of the three regions `text data bss`.

**Exam relevance:** "Where does an uninitialized static variable live?" → bss. "`char *s = "abc"; s[0]='x';`" → crash, whereas `char s[] = "abc"` can be modified (the array is on the stack).

### 10_switch_fallthrough — switch without break

**Question:** For x from 0 to 4, what does each line print?

**Expected output:**

```text
x = 0: default four 
x = 1: one two 
x = 2: two 
x = 3: three default four 
x = 4: four 
```

**Explanation:**

- `switch` jumps to the matching label, then runs **sequentially** until a `break` or the end of the block.
- x = 0 matches no case → jumps to `default` (even though it is in the middle), then falls into `case 4`.
- x = 4 jumps straight to `case 4` without passing through `default`.

**Exam relevance:** Exams like to put `default` in the middle or at the top. A `switch` expression must have an integer type (no strings/floats in C).

### 11_string_array_pointer — Strings: char arrays and pointers to string literals

**Question:** How do sizeof and strlen differ for a, b and p? What are sizeof("ab\0cd") and its strlen? What is sizeof('A') in C? What does d print?

**Expected output:**

```text
a: sizeof = 6, strlen = 5
b: sizeof = 10, strlen = 5
p: sizeof = 8, strlen = 5
c: sizeof = 5 (strlen(c) is UB: there is no '\0')
"ab\0cd": sizeof = 6, strlen = 2
sizeof('A') = 4, sizeof(char) = 1
in f: sizeof(s) = 8, strlen(s) = 5
a after the change = Bosch
d = abcdefg
strcmp: 1 1 1
a == "Bosch" ? 0, strcmp = 0
```

**Explanation:**

- `a` is a 6-byte array (5 characters + `'\0'`), `b` a 10-byte array (the rest is `'\0'`), `p` is just a pointer → 8 bytes on x86_64. `strlen` always counts up to the first `'\0'` → 5.
- `"ab\0cd"` has 5 characters + the automatic `'\0'` → sizeof 6, but strlen stops at the `\0` in the middle → 2.
- In **C**, `'A'` has type `int` → 4. In **C++** `'A'` is a `char` → 1.
- The parameter `char s[]` is really a `char *` → 8.
- `strncpy` adds no `'\0'` when the source is longer than n → you must set `d[7] = '\0'` yourself → 7 characters remain.
- `a == "Bosch"` compares **addresses**, not contents → 0. Compare contents with `strcmp` (returns 0 when equal).

**Exam relevance:** Distinguish `char s[] = "..."` (modifiable, on the stack) from `char *s = "..."` (a literal in .rodata; modifying it → segfault). A "sizeof vs strlen" question is almost always there.

### 12_precedence — Operator precedence and short-circuit evaluation

**Question:** What is the value on each line? After the || and && lines, what are i and k? What is arr[1] at the end?

**Expected output:**

```text
a & 4 == 4   -> 0
(a & 4) == 4 -> 1
1 << 2 + 1   -> 8
10 - 2 - 3   -> 5
2 * 3 % 4    -> 2
v = 10, w = 21, y = 21, arr[1] = 22
nested ?:   -> 1
(1, 2, 3)    -> 3
j = 1, i = 2
m = 0, k = 1
!5 = 0, !!5 = 1, ~5 = -6, -5 >> 1 = -3
```

**Explanation:**

- `a & 4 == 4` = `a & (4 == 4)` = `6 & 1` = 0. The classic trap when testing bits — always write `(a & MASK) == MASK`.
- `1 << 2 + 1` = `1 << 3` = 8, because `+` binds tighter than `<<`.
- `*p++` takes arr[0] = 10, then p points to arr[1]. `++*p` increments arr[1] to 21. `(*p)++` yields 21, then arr[1] becomes 22.
- The comma operator evaluates left to right and yields the last value → 3.
- `i++ || i++`: the left side yields 0 (i becomes 1) → the right side must be evaluated, yields 1 (i becomes 2) → j = 1.
- `k++ && k++`: the left side yields 0 → **stop**; the right side never runs → k = 1.
- `~5` = -6 (one's complement: ~x = -x - 1). `-5 >> 1` on a negative number is implementation-defined; gcc does an arithmetic shift → -3 (rounds down, unlike `-5 / 2 = -2`).

**Exam relevance:** Precedence to remember (high → low): `() [] -> .` › unary `! ~ ++ -- * & sizeof` › `* / %` › `+ -` › `<< >>` › `< <= > >=` › `== !=` › `&` › `^` › `|` › `&&` › `||` › `?:` › assignment › `,`.

### 13_function_pointer — Function pointers, arrays of function pointers and callbacks (qsort)

**Question:** What do the first three lines print? Is there any difference between fp(1, 2) and (*fp)(1, 2)? What order is the array in after qsort?

**Expected output:**

```text
7 + 3 = 10
7 - 3 = 4
7 * 3 = 21
fp(1, 2) = 3, (*fp)(1, 2) = 3, fp == add ? 1
sizeof(fp) = 8
5 4 3 2 1 
25 16 9 4 1 
```

**Explanation:**

- `ops` is an array of 3 function pointers of type `int (*)(int, int)`; `ops[i](7, 3)` calls the i-th function.
- `fp(1, 2)` and `(*fp)(1, 2)` are exactly equivalent; `add` and `&add` have the same value too.
- The qsort comparator returns negative/0/positive. `(a < b) - (a > b)` gives descending order and cannot overflow like `b - a`.
- `apply` takes a callback `int (*fn)(int)` → squares each element.

**Exam relevance:** Reading declarations: `int (*fp)(int)` is a pointer to a function; `int *fp(int)` is a FUNCTION returning `int*`; `int (*arr[3])(void)` is an array of 3 function pointers; `void (*signal(int, void (*)(int)))(int)` is the real prototype of `signal`. Use `typedef` for readability.

### 14_integer_promotion — Integer promotion: char/short automatically become int

**Question:** What is a + b, and c? Is sc == uc? Does the ~x == 0xF0 line print "equal" or "different"? What is sizeof(a + b)?

**Expected output:**

```text
a + b = 260
c = a + b -> 4
sizeof(a) = 1, sizeof(a + b) = 4, sizeof(+a) = 4
sc == uc ? 0  (sc -> -1, uc -> 255)
different: ~x = 0xFFFFFFF0
hi << 1 = 256, (uint8_t)(hi << 1) = 0
un-- from 0 = 4294967295
int sum = 60000, short sum = -5536
```

**Explanation:**

- `a + b`: both are promoted to `int` before adding → 260, no overflow. Only the assignment to `unsigned char` truncates: 260 mod 256 = 4.
- `sc == uc`: -1 promoted to int is still -1, 255 promoted is 255 → different, even though both have the bit pattern `0xFF`.
- `~x`: x = 0x0F becomes the int 0x0000000F; inverting gives 0xFFFFFFF0 ≠ 0xF0. Write `(uint8_t)~x`.
- `hi << 1` is computed as an int → 256; truncated to uint8_t → 0.
- Unsigned overflow is well defined (modulo 2ⁿ) → 0 - 1 = 4294967295. Signed overflow is UB.
- 60000 does not fit in a `short` → an implementation-defined conversion; gcc wraps modulo → 60000 - 65536 = -5536.

**Exam relevance:** A real trap in driver code: `uint16_t a = 0xFFFF; a * a` → promoted to **signed int**, which overflows → UB. Rules: types smaller than int → int; mixing signed/unsigned of the same rank → unsigned.

### 15_float — Floating point: comparison, rounding, integer division

**Question:** Is 0.1 + 0.2 == 0.3? Is f == 0.1? What are x, y and z? (int)-3.99? What does 16777217.0f print?

**Expected output:**

```text
0.1 + 0.2 == 0.3 ? 0
0.1 + 0.2 = 0.30000000000000004
|d - 0.3| < 1e-9 ? 1
f == 0.1 ? 0, f == 0.1f ? 1
x = 3, y = 3.0, z = 3.5
(int)3.99 = 3, (int)-3.99 = -3
16777217.0f = 16777216.0
inf = inf, nan == nan ? 0, inf > 1e308 ? 1
sizeof: float = 4, double = 8, long double = 16
```

**Explanation:**

- 0.1, 0.2 and 0.3 cannot be represented exactly in binary → the sum is slightly off. Compare floating point with `fabs(a - b) < eps`.
- `f == 0.1`: f is promoted to double, but the float value of 0.1 differs from the double value of 0.1 → 0.
- `7 / 2` is **integer** division → 3; stored in a double it is still 3.0. One floating-point operand is enough → 3.5.
- Casting to int truncates toward zero: -3.99 → -3 (not -4).
- float has a 24-bit mantissa → integers above 2²⁴ = 16777216 are no longer exact.
- NaN is not equal to itself → test for NaN with `x != x` or `isnan`.

**Exam relevance:** Embedded code often avoids float (slow without an FPU) → use fixed point. Never use `==` on floating-point values or a float as a loop counter.

### 16_scope_storage — Variable scope, shadowing and static inside a loop

**Question:** Which number does each x= line print? What do the three calls to f() print? How do s and t print in the last for loop?

**Expected output:**

```text
x = 1
x = 3 (inner block)
x = 1 (extern)
x = 2
f: n = 1, m = 1
f: n = 2, m = 1
f: n = 3, m = 1
s = 11, t = 11
s = 12, t = 11
s = 13, t = 11
```

**Explanation:**

- The first line runs before `int x = 2` is declared → it uses the global = 1.
- Inside the block, `x = 3` shadows main's x. `extern int x` in the nested block brings back the global.
- `static int n` is zero-initialized exactly once and keeps its value between calls; `m` is automatic and re-initialized on every call.
- `static int s = 10` inside the for body is also initialized only once → 11, 12, 13; `t` is always 11.

**Exam relevance:** The 4 storage classes: `auto` (default, stack), `static`, `extern`, `register`. Distinguish **scope** (where it is visible) from **lifetime** (how long it lives): a static local has block scope but lives for the whole program.

## 02-cpp

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

## 03-process

### 01_fork_count — fork() three times ⚠️ nondeterministic

**Question:** How many "PID = ..." lines are printed? How many different PPIDs are there?

**Expected output:**

```text
8 lines in an unpredictable order, e.g.:
PID = 5001, PPID = 4990     <- the original process (PPID = shell/make)
PID = 5002, PPID = 5001
PID = 5003, PPID = 5001
PID = 5005, PPID = 5001
PID = 5004, PPID = 5002
PID = 5006, PPID = 5002
PID = 5007, PPID = 5003
PID = 5008, PPID = 5004
```

**Explanation:**

- 1 → 2 → 4 → 8 processes. The tree: the root has 3 children, the first child has 2 children, ...
- There are 5 different PPIDs (the root's parent, the root, and the 3 other processes that have children: every process created by the 1st or 2nd fork has children).
- The print order depends on the scheduler → **nondeterministic**.

**Exam relevance:** Formula: n consecutive `fork()` calls → 2^n processes, and a `printf` afterwards prints 2^n times; the number of **child processes created** = 2^n − 1.

### 02_fork_if — fork() inside if and inside logical expressions ⚠️ nondeterministic

**Question:** How many lines does part [1] print? How many does part [2] print with fork() && fork() || fork()?

**Expected output:**

```text
[1] appears 3 times
-----
[2] appears 5 times
(PIDs and line order vary)
```

**Explanation:**

- Part 1: the root forks C1 (the parent gets > 0, so it does not enter the if). C1 gets 0 → forks again, creating C2. 3 processes in total.
- Part 2, calling the three calls A, B, C:
  - Root: A > 0 → runs B (creates P2). In the root B > 0 → `&&` is true → C is skipped.
  - P2: B = 0 → `&&` is false → runs C (creates P3).
  - P1 (A's child): A = 0 → skips B, runs C (creates P4).
  - Total: root, P1, P2, P3, P4 = **5**.

**Exam relevance:** "How many processes are created" questions are very common. Draw the tree and write the return value (0 or > 0) on each branch.

### 03_fork_buffer — stdio buffers and fork

**Question:** What appears on screen? How many times does the letter A appear?

**Expected output:**

```text
AB
AB
```

**Explanation:**

- `"A"` has no `\n`, so it is still in `stdout`'s buffer. `fork()` copies all of user-space memory, **buffer included** → both parent and child have an "A" waiting to be printed.
- Fix: `fflush(stdout)` before `fork()` → "A" is printed once: `AB` then `B`.
- With `printf("A\n")`: on a terminal it prints `A, B, B`; but redirected to a file (fully buffered) it becomes `A B A B`.
- `write(1, ...)` is a system call with no user-space buffer → nothing is duplicated.

**Exam relevance:** "Why is the output duplicated when redirected to a file?" → the stdio buffer is duplicated by `fork`. Remember: `exit()` flushes buffers, `_exit()` does not.

### 04_fork_exec_wait — fork + exec + waitpid

**Question:** What exit code (WEXITSTATUS) does the parent print? Does the perror("execlp") line run?

**Expected output:**

```text
Child (pid 6120) will run ls
ls: cannot access '/does_not_exist': No such file or directory
Parent: child 6120 exited with code 2
```

**Explanation:**

- `execlp` succeeds (it finds `ls` in PATH) → the child becomes `ls`, and the lines after `execlp` **never run**.
- `ls` exits with code 2 on a serious error (an argument it cannot access). A valid directory → 0.
- With `lsx`: exec fails → `perror` prints `execlp: No such file or directory` → exit code 127 (the shell's convention for "command not found").
- Only the low 8 bits of the exit code are kept: `exit(256)` → the parent sees 0.

**Exam relevance:** The suffixes: **l** = argument list, **v** = `argv` array, **p** = search PATH, **e** = pass an environment. They all end up calling the `execve` system call.

### 05_zombie — Zombie processes

**Question:** During those 30 seconds, in a second terminal, what does the STAT column show for the child? How is its command name displayed?

**Expected output:**

```text
Terminal 1:
Parent 7001: sleeping 30s WITHOUT calling wait(). Child = 7002
Terminal 2: ps -o pid,ppid,stat,cmd --ppid 7001
Child 7002: exiting right away
(after 30 seconds) Parent exits

Terminal 2:
    PID    PPID STAT CMD
   7002    7001 Z+   [05_zombie] <defunct>
```

**Explanation:**

- STAT `Z` = zombie, and the command name shows `<defunct>`. A zombie uses no CPU/RAM but holds a **PID** — too many of them exhaust the PIDs.
- `kill -9` on a zombie **has no effect**: it is already dead.
- When the parent exits, the zombie becomes an orphan → it is adopted by `init/systemd` (or a subreaper), which calls `wait` to clean it up.
- How to avoid it: call `wait/waitpid`, handle `SIGCHLD`, or `signal(SIGCHLD, SIG_IGN)`.

**Exam relevance:** Process states in `ps`: R (running), S (interruptible sleep), D (uninterruptible sleep, usually waiting for I/O), T (stopped), Z (zombie).

### 06_orphan — Orphan processes ⚠️ nondeterministic

**Question:** How do the PPIDs the child prints on the "before" and "after" lines differ? Where does the shell prompt appear?

**Expected output:**

```text
Parent 8100: exiting after 1s
Child 8101: before, PPID = 8100
user@ubuntu:~/bosch-lab$ Child 8101: after,  PPID = 1      <- or the PID of "systemd --user"
```

**Explanation:**

- At first the PPID is the parent's PID. The parent exits after 1 second → the child becomes an orphan and is adopted.
- On an Ubuntu desktop the PPID is usually the PID of `systemd --user` (a **subreaper**), not necessarily 1. In Docker/WSL it may be 1.
- The shell only waits for the parent, so the prompt appears **before** the child's "after" line. (In the web terminal the run ends when the parent exits, so the "after" line may not be shown.)

**Exam relevance:** Zombie: the child is dead, the parent is alive but does not wait. Orphan: the parent is dead, the child is alive. Orphans are harmless; accumulating zombies are not.

### 07_fork_cow — Variables after fork(): copy-on-write ⚠️ nondeterministic

**Question:** What do g, local and heap print in the parent after the child incremented all three? Are the &g addresses the same in parent and child?

**Expected output:**

```text
child:  g=11 local=21 heap=31 | &g=0x5870e837b010 heap=0x587110e802a0
parent: g=10 local=20 heap=30 | &g=0x5870e837b010 heap=0x587110e802a0
(the addresses change on every run, but they are identical in parent and child)
```

**Explanation:**

- The child modifies its own copy; the parent still sees the old values 10/20/30.
- The printed addresses are **identical** because they are virtual addresses — the child has its own page table pointing to its own physical pages. The concrete numbers change on every run (ASLR), but parent and child always match.
- Copy-on-write: after fork, the two processes share physical pages read-only; only when one of them writes does the kernel copy that page. That is why fork is fast.

**Exam relevance:** A child inherits: a copy of memory, open fds, signal handlers, cwd, environment variables. A child does NOT inherit: the PID, pending signals, file locks (fcntl), timers (alarm), other threads.

### 08_fork_fd_offset — File descriptors after fork(): a shared offset

**Question:** What does file 1 contain (parent and child share an fd opened BEFORE fork)? What does file 2 contain (each side opens it on its own AFTER fork)?

**Expected output:**

```text
/tmp/bosch_offset1.txt (13 bytes):
child
parent
/tmp/bosch_offset2.txt (9 bytes):
parent
X
```

**Explanation:**

- File 1: the fd is duplicated, but both copies point to the same open file description → a shared offset. The child writes 6 bytes, the offset becomes 6, and the parent continues writing from 6 → "child\nparent\n".
- File 2: each side `open`s it separately → separate offsets starting at 0. The parent's "parent\n" (7 bytes) overwrites the first 7 bytes of "XXXXXXXX\n", leaving "parent\nX\n".

**Exam relevance:** Three levels: the fd table (per process) → the open file table (offset, flags; shared after fork/dup) → the inode. `dup2(fd, 1)` is how the shell implements redirection.

### 09_exec_buffer — exec() replaces the program: the PID stays, the stdio buffer is lost ⚠️ nondeterministic

**Question:** Does the "no newline yet..." text appear? Does the PID change after exec? Is the "never printed" line printed?

**Expected output:**

```text
PID before exec = 10144
PID after exec  = 10144
(the PID changes on every run, but the two lines always show the same number)
```

**Explanation:**

- The first line ends in `\n` and stdout is a terminal (line-buffered) → it was flushed before exec.
- "no newline yet..." is still in the stdio buffer (user-space memory) → exec wipes all the old memory → it is lost.
- `$$` in `sh -c` is sh's PID = the old PID, because exec does not create a new process.
- A successful exec never returns → the last two lines do not run.

**Exam relevance:** fork creates a new process (a copy); exec replaces the insides of the current process. A shell runs a command with fork + exec in the child + wait in the parent. `vfork`/`posix_spawn` optimize the exec-right-after-fork case.

## 04-signal

### 01_sigint_handler — Catching Ctrl+C with sigaction

**Question:** What happens on the 1st and 2nd Ctrl+C? On which press does the program exit, and what does the last line print?

**Expected output:**

```text
PID 9001. Press Ctrl+C 3 times to quit.
running... count = 0
running... count = 0
^C [handler: got SIGINT]
running... count = 1
^C [handler: got SIGINT]
running... count = 2
^C [handler: got SIGINT]
Clean exit after 3 Ctrl+C presses
```

**Explanation:**

- Ctrl+C makes the terminal send **SIGINT** (2) to the whole foreground process group. The handler replaces the default action (terminate).
- `sleep()` is interrupted by the signal and returns early, so the loop checks `stop` right away.
- `volatile`: forces the variable to be re-read from memory every time (not cached in a register). `sig_atomic_t`: reads/writes are atomic with respect to signals.
- `printf` is not async-signal-safe (it uses a lock and an internal buffer) → the handler uses `write`.

**Exam relevance:** The old `signal()` behaves differently between systems; `sigaction()` is the POSIX standard and lets you set `sa_mask`, `SA_RESTART`, `SA_SIGINFO`.

### 02_sigchld_reap — Reaping zombies with SIGCHLD ⚠️ nondeterministic

**Question:** Does the program print "Reaped all 5 children"? If the handler called waitpid only once (no while loop), what could go wrong?

**Expected output:**

```text
(the number of lines in the middle may vary between runs)
parent working... reaped 0 children
parent working... reaped 1 children
parent working... reaped 2 children
parent working... reaped 3 children
parent working... reaped 4 children
Reaped all 5 children, no zombies left
```

**Explanation:**

- Every time SIGCHLD arrives, `sleep` is interrupted, so the parent prints its progress again.
- `WNOHANG`: if no child has exited, return 0 immediately (do not block). `-1` = any child.
- With only one `waitpid`: when 2 children exit almost at once, the second SIGCHLD is merged (pending is just 1 bit) → one child stays a zombie and `reaped` never reaches 5 → it hangs.
- Save/restore `errno`, because the handler may interrupt code that is checking `errno`.

**Exam relevance:** Standard signals are not queued; real-time signals (`SIGRTMIN..SIGRTMAX`) are. SIGCHLD's default action is **ignore**.

### 03_kill_test — Sending signals with the kill command

**Question:** What does the first line print (sigaction for SIGKILL)? For each command kill -TERM, -USR1, -STOP, -CONT, -KILL from terminal 2: how does the program react?

**Expected output:**

```text
sigaction(SIGKILL): Invalid argument
PID = 9500. In terminal 2, try in turn:
  ...
tick 0
tick 1
[handler] SIGTERM -> still running     <- kill -TERM
tick 2
[handler] SIGUSR1                      <- kill -USR1
tick 3
                                       <- kill -STOP: the ticks stop (ps shows STAT = T)
tick 4                                 <- kill -CONT: it continues
Killed                                 <- kill -KILL: dies at once, the shell prints "Killed"
```

**Explanation:**

- `sigaction(SIGKILL, ...)` returns -1 with `errno = EINVAL` → prints `Invalid argument`.
- SIGTERM is caught, so the process does **not** die. That is why `kill <pid>` (TERM by default) sometimes fails to kill a process.
- SIGSTOP stops the process (it cannot be caught), SIGCONT lets it continue.
- SIGKILL is handled directly by the kernel; the program gets no chance to clean up.

**Exam relevance:** Learn the numbers on Linux x86: HUP 1, INT 2, QUIT 3, ABRT 6, KILL 9, USR1 10, SEGV 11, USR2 12, PIPE 13, ALRM 14, TERM 15, CHLD 17, CONT 18, STOP 19, TSTP 20.

### 04_alarm_pause — alarm, pause and the default action of SIGALRM

**Question:** What does alarm(0) return? What do pause() and errno return? Does the last line "not printed" appear, and what does the shell report?

**Expected output:**

```text
alarm(0) returns 10
alarm(1), then pause()...
  [handler] SIGALRM
pause() = -1, errno = Interrupted system call
alarm(1) again, this time WITHOUT catching SIGALRM
Alarm clock
```

**Explanation:**

- `alarm(0)` cancels the pending timer and returns the seconds that were left (10).
- After 1 second SIGALRM arrives → the handler runs → pause() returns -1 with `errno = EINTR`.
- The second time SIGALRM is not caught → its default action **terminates** the process. The shell prints "Alarm clock" and `echo $?` = 142 (128 + 14). "not printed" does not appear.

**Exam relevance:** Default actions: Term (SIGINT, SIGTERM, SIGALRM, SIGPIPE, SIGUSR1/2, SIGHUP, SIGKILL), Core (SIGSEGV, SIGABRT, SIGFPE, SIGQUIT), Ign (SIGCHLD), Stop (SIGSTOP, SIGTSTP), Cont (SIGCONT). The exit code after dying from signal n is 128 + n.

### 05_sigprocmask — Blocking signals (sigprocmask), pending signals, and why signals are not queued

**Question:** SIGUSR1 is sent 3 times while blocked → after unblocking, how many times does the handler run? And with the real-time signal SIGRTMIN?

**Expected output:**

```text
blocked: SIGUSR1 pending? 1, SIGRTMIN pending? 1
before unblocking: usr1 = 0, rt = 0
after unblocking:  usr1 = 1, rt = 3
```

**Explanation:**

- A blocked signal is not delivered; it becomes pending → the handlers have not run yet (0, 0).
- A standard signal has just one pending bit → the 3 raises merge into 1.
- Real-time signals are queued → delivered all 3 times, and they can carry data (`sigqueue`).
- Pending signals are delivered as soon as `sigprocmask(SIG_UNBLOCK)` returns.

**Exam relevance:** Distinguish **blocking** (postponed, still pending) from **ignoring** (thrown away). SIGKILL and SIGSTOP cannot be blocked, ignored or caught. In threads use `pthread_sigmask`.

## 05-thread

### 01_race_condition — A race condition with counter++ ⚠️ nondeterministic

**Question:** Does the printed counter equal 2000000? Is it the same over 3 runs?

**Expected output:**

```text
counter = 1087342 (expected 2000000)     <- less than 2000000, different on every run
```

**Explanation:**

- Thread 1 reads counter = 5, thread 2 also reads 5, both write 6 → one increment is lost (a lost update).
- The result is **nondeterministic**; on a single-core machine or with a small N it may happen to be right — the race condition is still there.
- `volatile` does **not** fix it (it only prevents optimizations; it does not make anything atomic).

**Exam relevance:** Three fixes: a mutex, an atomic variable (`__atomic_fetch_add`, `stdatomic.h`, `std::atomic`), or let each thread count separately and add up after joining.

### 02_mutex_fix — Fixing the race with pthread_mutex

**Question:** What is counter now? Is it slower or faster than lab 01?

**Expected output:**

```text
counter = 2000000 (expected 2000000)
```

**Explanation:**

- Always exactly 2000000. It runs noticeably **slower** because of 2 million lock/unlock pairs and contention between CPU cores.
- Linux implements mutexes with **futexes**: an uncontended lock stays entirely in user space; only contention enters the kernel.
- Hold locks as briefly as possible, but locking in tiny pieces also has a cost.

**Exam relevance:** A mutex has **ownership**: the thread that locks it unlocks it. Locking a normal (default) mutex twice in one thread → deadlock; use a `RECURSIVE` mutex if you need that.

### 03_deadlock — Deadlock from locking in opposite orders

**Question:** Which lines does the program print, and then what? Does the "No deadlock" line appear?

**Expected output:**

```text
T1: holding A, waiting for B...
T2: holding B, waiting for A...
(frozen forever — Ctrl+C to quit; the two lines may appear in either order)
```

**Explanation:**

- All 4 Coffman conditions hold: **mutual exclusion**, **hold and wait**, **no preemption**, **circular wait**.
- The `usleep` only makes the deadlock near-certain; without it the program can still deadlock, just occasionally.
- Debugging: `gdb -p <PID>`, then `thread apply all bt` shows both threads stuck in `__lll_lock_wait`.
- Fixes: lock in a fixed order (breaks the circular wait), `trylock` + back-off, or `std::scoped_lock(a, b)` in C++17.

**Exam relevance:** Livelock: threads keep running but keep yielding to each other and make no progress. Starvation: a thread never gets the resource.

### 04_cond_var — A one-slot producer–consumer with a condition variable

**Question:** In what order do the produce / consume lines appear? Can two produce lines appear in a row?

**Expected output:**

```text
produce 10
    consume 10
produce 20
    consume 20
produce 30
    consume 30
produce 40
    consume 40
produce 50
    consume 50
```

**Explanation:**

- The slot holds 1 item: the producer must wait for `full == 0`, the consumer for `full == 1` → strict alternation; two produce lines in a row are **impossible**.
- `pthread_cond_signal` has no "memory": if nobody is waiting, the signal is lost. So the real state lives in the `full` variable, protected by the mutex.
- `while` is needed because of spurious wakeups, or because another thread may already have taken the data.

**Exam relevance:** A condition variable always goes with 1 mutex + 1 condition (predicate). `signal` wakes at least 1 thread, `broadcast` wakes them all.

### 05_semaphore — A semaphore that limits threads to 2 at a time ⚠️ nondeterministic

**Question:** What is the "max at the same time" value? Is the enter/leave order fixed?

**Expected output:**

```text
(the order varies, e.g.)
T0 enters (inside: 1)
T1 enters (inside: 2)
T0 leaves
T2 enters (inside: 2)
T1 leaves
T3 enters (inside: 2)
...
Max threads at the same time = 2
```

**Explanation:**

- Never more than 2 threads are between `sem_wait` and `sem_post` → max = 2.
- The separate mutex `m` only protects the `inside` counter and the `printf`.
- The 2nd argument of `sem_init` = 0: shared between threads; ≠ 0: shared between processes (the semaphore must then live in shared memory).

**Exam relevance:** Mutex vs semaphore: a mutex has an owner and is for mutual exclusion; a semaphore is a counter that another thread can `post`, used for signaling/limiting resources. Named semaphores: `sem_open("/name", ...)`.

### 06_cpp_thread — std::thread, std::mutex, lock_guard, std::atomic ⚠️ nondeterministic

**Question:** What are plain, guarded and atomic? Which one can be wrong?

**Expected output:**

```text
plain   = 1203117      <- wrong, changes on every run
guarded = 2000000
atomic  = 2000000
CPU cores = 8          <- depends on the machine
```

**Explanation:**

- `plain` is the same race condition as lab 01.
- `lock_guard` locks in its constructor and unlocks in its destructor → you cannot forget to unlock, even with exceptions.
- `atomic<int>` uses atomic CPU instructions (x86: `lock xadd`), usually faster than a mutex for simple operations.
- A `std::thread` that is still **joinable** when destroyed → `std::terminate()` → the program aborts.

**Exam relevance:** `unique_lock` is more flexible than `lock_guard` (unlock midway, works with `condition_variable`). C++20's `std::jthread` joins automatically when destroyed.

### 07_thread_arg_bug — Passing &i to threads, and getting results back through pthread_join ⚠️ nondeterministic

**Question:** Which ids does the "WRONG" part print? What does the "RIGHT" part print and what is the sum?

**Expected output:**

```text
  WRONG: thread got id = 2
  WRONG: thread got id = 3
  WRONG: thread got id = 4
  WRONG: thread got id = 2
  RIGHT: thread 0 returned 0
  RIGHT: thread 1 returned 1
  RIGHT: thread 2 returned 4
  RIGHT: thread 3 returned 9
sum = 14
```

**Explanation:**

- The WRONG part: each thread reads `*arg` some time **after** it was created; by then main may already have incremented i, or even left the loop (value 4, or the memory reused by the next loop). Every run differs, and the line order changes too.
- The RIGHT part: the id is passed **by value** inside the pointer itself → each thread has its own copy. Results come back through `pthread_join(t, &ret)` in join order → fixed: 0 + 1 + 4 + 9 = 14.

**Exam relevance:** Passing `&i` to threads is a favorite "find the bug" question. A thread returning a pointer to its own local variable is wrong (its stack is gone) — return malloc'd memory or a value cast to a pointer.

### 08_thread_local — What threads share and what they own: globals, thread-locals, PID/TID

**Question:** What mine and shared values does each thread print? What does main see at the end? Is a thread's PID equal to main's?

**Expected output:**

```text
thread 0: mine = 3, shared = 3, getpid()==main? 1, tid==pid? 0
thread 1: mine = 3, shared = 6, getpid()==main? 1, tid==pid? 0
main: mine = 0, shared = 6, tid==pid? 1
```

**Explanation:**

- `shared` is a global used by everyone → it accumulates 3, then 6.
- `mine` is `_Thread_local` → each thread has its own copy starting at 0; main's copy is untouched → 0.
- `getpid()` is the same in every thread (the TGID). The other threads' TIDs differ from the PID; only the main thread has TID = PID.

**Exam relevance:** Threads share: code, data, heap, fds, signal handlers, cwd, PID. Each has its own: stack, registers/PC, TID, signal mask, errno (errno is thread-local). On Linux both processes and threads are created with `clone()` using different sharing flags.

## 06-ipc

### 01_pipe — pipe: the parent writes, the child reads

**Question:** How many bytes does the child read? What does read return the last time? What if the child does NOT close fd[1]?

**Expected output:**

```text
Child read 17 bytes: hello from parent
Child: read returned 0
Parent: done

If the child does not close(fd[1]): after the first line the program HANGS (the child waits in read forever).
```

**Explanation:**

- `"hello from parent"` is 17 characters; `strlen` does not count the `'\0'`.
- Once the parent closes the write end, `read` returns 0 = EOF → the loop ends.
- If the child still holds `fd[1]`, the kernel sees a remaining "writer" (the child itself), so `read` blocks forever → a self-inflicted deadlock.
- Writing to a pipe with no reader left → **SIGPIPE** (deadly by default), or `write` returns `EPIPE` if the signal is ignored.

**Exam relevance:** A pipe is one-way and only works between related processes (the fds are inherited through fork). Writes ≤ `PIPE_BUF` (4096 bytes on Linux) are atomic. The default capacity is 64 KiB.

### 02_pipe_dup2 — Implementing ls / | wc -l with dup2 ⚠️ nondeterministic

**Question:** What number does the program print (compared with running ls / | wc -l in the shell)? What if the parent forgets to close fd[1]?

**Expected output:**

```text
22                                  <- exactly what "ls / | wc -l" prints on your machine
Done: same as  ls / | wc -l

If the parent does not close fd[1]: wc never sees EOF → it hangs.
```

**Explanation:**

- Child 1: stdout (fd 1) → the pipe's write end, then `exec ls`. Child 2: stdin (fd 0) ← the read end, then `exec wc -l`.
- File descriptors survive `exec`, so `ls` and `wc` have no idea they are using a pipe.
- Every copy of the write end must be closed before `wc` gets EOF: child 1 (once `ls` exits), child 2 and **the parent too**.

**Exam relevance:** `dup(fd)` returns the lowest free fd. `2>&1` in the shell is exactly `dup2(1, 2)`. The order of shell redirections matters because it is a sequence of dup2 calls.

### 03_fifo — A named pipe (FIFO) between 2 programs

**Question:** When only the reader runs, at which line does it stop? When the writer presses Ctrl+D, what does the reader print and how does it end?

**Expected output:**

```text
Terminal 1 (reader, started first):
Reader: opening the FIFO...             <- stops here until the writer opens it
Reader: opened, waiting for data
Reader got: hello
Reader got: bosch
Reader: read returned 0, exiting

Terminal 2 (writer):
Writer: opening the FIFO...
Writer: opened. Type a few lines, then Ctrl+D to finish
hello
bosch
^D
Writer: closed the FIFO
```

**Explanation:**

- Opening the read end blocks until there is a writer (and vice versa) — the default behavior without `O_NONBLOCK`.
- The writer closes its fd → the reader gets EOF (read = 0).
- `ls -l` shows `p` (pipe) as the first character. No data is written to disk; the file is just a "meeting point" in the filesystem.

**Exam relevance:** File type characters in `ls -l`: `-` regular file, `d` directory, `l` symlink, `c` char device, `b` block device, `p` FIFO, `s` socket.

### 04_shm — POSIX shared memory between a parent and a child

**Question:** Which string does the parent read in mem? What is the local variable in the parent?

**Expected output:**

```text
Child:  mem = "child wrote to shm", local = 99
Parent: mem = "child wrote to shm", local = 1
```

**Explanation:**

- Shared memory is the **fastest** IPC: once mapped, reads and writes are ordinary memory accesses with no system calls → but you must synchronize yourself (a semaphore, or a mutex with `PTHREAD_PROCESS_SHARED`).
- `local` lives in private memory, copy-on-write after `fork` → the parent still sees 1.
- POSIX shm objects live in `/dev/shm` (tmpfs). `close(fd)` does not remove the mapping; you need `munmap` and `shm_unlink`.
- Older Ubuntu (glibc < 2.34) needs `-lrt` at link time.

**Exam relevance:** System V: `shmget / shmat / shmdt / shmctl(IPC_RMID)`, listed with `ipcs -m`. POSIX: `shm_open / ftruncate / mmap / munmap / shm_unlink`.

### 05_mmap_file — Mapping a file into memory with mmap

**Question:** What does the "After" line print? How many bytes is the file?

**Expected output:**

```text
Before: hello mmap world
After : HELLO MMAP world
File size = 17 bytes
```

**Explanation:**

- 16 characters + `'\n'` = 17 bytes.
- Writing straight into the mapped memory changes the file, because the mapping is `MAP_SHARED`. With `MAP_PRIVATE` the change only lands in a private copy-on-write page and the file is unchanged.
- `mmap` works in **pages** (4096 bytes): accessing past the file but inside the last page reads 0 and is never saved; going past that page → **SIGBUS/SIGSEGV**.

**Exam relevance:** mmap's advantages: no copying between kernel and user buffers, convenient random access. `MAP_ANONYMOUS` creates memory with no file behind it (malloc uses it for large blocks).

### 06_unix_socket — UNIX domain socket: a server/client pair using fork

**Question:** In what order are the two lines printed? Why must bind/listen happen BEFORE fork?

**Expected output:**

```text
Server got: ping
Client got: pong
```

**Explanation:**

- The client only prints after receiving "pong", and the server only sends "pong" after printing → a fixed order.
- If you forked first and bound/listened afterwards, the client could `connect` before the socket is listening → `ECONNREFUSED`/`ENOENT`.
- The socket file remains after the program exits → the next `bind` fails with **Address already in use** unless you `unlink` it.
- `socketpair(AF_UNIX, ...)` creates a ready-connected pair of sockets, handy for parent–child.

**Exam relevance:** `SOCK_STREAM` (TCP-like, reliable, ordered, a byte stream) vs `SOCK_DGRAM` (UDP-like, keeps message boundaries). UNIX sockets can even pass file descriptors (SCM_RIGHTS).

### 07_mqueue — POSIX message queues and priorities

**Question:** The child sends 3 messages with priorities 1, 5 and 3. In what order does the parent receive them? What is mq_curmsgs before receiving?

**Expected output:**

```text
messages waiting: 3
received "high" (prio 5)
received "medium" (prio 3)
received "low" (prio 1)
```

**Explanation:**

- The child has exited but the 3 messages are still in the queue, because the queue is a kernel object independent of the sending process.
- `mq_receive` always takes the highest-priority message → 5, 3, 1.
- Each receive takes exactly one whole message (boundaries are kept), unlike a pipe, which is a continuous byte stream.

**Exam relevance:** POSIX mq (`mq_open`, names start with `/`, link with `-lrt` on old glibc) and System V mq (`msgget`, `msgsnd`, `msgrcv`, selecting messages by `mtype`). Inspect with `ls /dev/mqueue` and `ipcs -q`.

### 08_sigpipe — Writing to a pipe with no reader: SIGPIPE and EPIPE

**Question:** With SIGPIPE ignored, what does write return? With the default action, how does the child end? What is the default pipe capacity on Linux?

**Expected output:**

```text
pipe capacity = 65536 bytes
SIGPIPE ignored: write = -1, errno = Broken pipe
child killed by signal 13 (Broken pipe)
1st read = 2, 2nd read = 0 (EOF)
```

**Explanation:**

- A Linux pipe holds 64 KiB by default; when it is full, write blocks (or returns EAGAIN with O_NONBLOCK).
- The read end is closed: with SIGPIPE ignored, write returns -1/EPIPE.
- The child restores SIG_DFL → it gets SIGPIPE (13) and dies inside write, so the "not printed" line never runs.
- Reading after every write end is closed: the remaining data (2 bytes) comes first, then 0 = EOF.

**Exam relevance:** Network servers usually do `signal(SIGPIPE, SIG_IGN)` or use `send(..., MSG_NOSIGNAL)` so they do not die when a client disconnects. Always close unused pipe ends after fork.

## 07-kernel-module

### hello — Hello kernel module

**Question:** After insmod, what does dmesg | tail print? What about sudo insmod hello.ko count=3? What does rmmod print?

**Expected output:**

```text
$ sudo apt install build-essential linux-headers-$(uname -r)
$ make
$ sudo insmod hello.ko
$ dmesg | tail -2
[12345.678901] hello: hello kernel (0)
$ lsmod | grep hello
hello                  16384  0
$ sudo rmmod hello
$ dmesg | tail -1
[12350.123456] hello: goodbye kernel

$ sudo insmod hello.ko count=3   -> prints 3 lines (0), (1), (2)

With Secure Boot on: insmod: ERROR: could not insert module hello.ko: Key was rejected by service
(being able to build hello.ko is enough for exam preparation)
```

**Explanation:**

- `module_init/module_exit` declare the entry and exit points. `__init` lets the kernel free the init code after loading.
- Without `MODULE_LICENSE("GPL")` the kernel becomes "tainted" and the module cannot use `EXPORT_SYMBOL_GPL` symbols.
- `insmod` loads exactly the given file; `modprobe` searches `/lib/modules` and loads dependencies automatically.
- If init returns a negative value (e.g. `-ENOMEM`), insmod fails and the module is not loaded.

**Exam relevance:** Common questions: the command that lists loaded modules (`lsmod` or `cat /proc/modules`), how to read the kernel log (`dmesg`), the module file extension (`.ko`), the kernel's logging function (`printk`).

## 08-shell

### Drill 01

Create a file `run.sh`, give it permissions `754`, and check with `ls -l` and `stat`. What permission string is shown? Who may do what?

```sh
touch run.sh
chmod 754 run.sh
ls -l run.sh                 # -rwxr-xr-- 1 user user 0 ... run.sh
stat -c '%a %A %U' run.sh    # 754 -rwxr-xr-- user
```
7 = rwx (owner), 5 = r-x (group), 4 = r-- (others). r = 4, w = 2, x = 1.

### Drill 02

Show the current `umask`. With `umask 022`, what permissions does a new file created by `touch` get, and a new directory created by `mkdir`?

```sh
umask            # normal Ubuntu user: 0002; root: 0022
umask 022
touch f; mkdir d
ls -ld f d       # -rw-r--r-- f  (644)   drwxr-xr-x d  (755)
```
Files start from 666 and directories from 777, then the bits in the umask are **removed**: 666 & ~022 = 644, 777 & ~022 = 755.

### Drill 03

Create `a.txt`, a hard link `hard.txt` and a soft link `soft.txt` to it. Delete `a.txt`. What does `cat` print for the two links? What does `ls -li` show about the inodes?

```sh
echo hello > a.txt
ln a.txt hard.txt
ln -s a.txt soft.txt
ls -li            # a.txt and hard.txt have the SAME inode number, link count = 2; soft.txt has a different inode
rm a.txt
cat hard.txt      # hello  (the data survives because the inode still has 1 link)
cat soft.txt      # cat: soft.txt: No such file or directory  (a dangling symlink)
```
A hard link = another name for the same inode (not allowed for directories, cannot cross filesystems). A soft link = a file containing a path.

### Drill 04

Use `ls /etc/hostname /does_not_exist` (it prints to both stdout and stderr). Compare `cmd > f 2>&1` with `cmd 2>&1 > f`: where does the error go?

```sh
ls /etc/hostname /does_not_exist > f1 2>&1
cat f1            # both the error line and /etc/hostname are in f1; nothing on screen
ls /etc/hostname /does_not_exist 2>&1 > f2
                  # screen: ls: cannot access '/does_not_exist': No such file or directory
cat f2            # only /etc/hostname
```
Redirections are processed **left to right**. `2>&1` means "make fd 2 point where fd 1 points **right now**". In the second form, fd 1 is still the terminal at that moment.

### Drill 05

Distinguish `>`, `>>`, `2>/dev/null`, `&>` and `tee`. Write the output of `date` to `log.txt` while still seeing it on screen, without overwriting the old content.

```sh
date > log.txt        # overwrite
date >> log.txt       # append
find / -name passwd 2>/dev/null   # drop error messages
ls /x /etc &> all.txt # stdout + stderr into the same file (bash)
date | tee -a log.txt # show on screen and append to the file
```

### Drill 06

In the `bosch-lab` directory, find every line containing `fork` in the `.c` files, with line numbers. Then: count the files containing `pthread`, search case-insensitively, and print the lines of a file that do NOT contain `#include`.

```sh
grep -rn "fork" --include="*.c" .
grep -rl "pthread" . | wc -l
grep -rni "mutex" .
grep -v "#include" 05-thread/02_mutex_fix.c
grep -c "printf" 01-c/05_bitwise.c      # number of matching LINES
```
`-r` recursive, `-n` line numbers, `-i` ignore case, `-v` invert, `-l` file names only, `-c` count, `-w` whole words, `-E` extended regex.

### Drill 07

Use `find`: find every `.cpp` file; find files larger than 1 MB in `/var/log`; find files modified in the last day; delete every `.o` file under `build/`.

```sh
find . -name "*.cpp"
sudo find /var/log -type f -size +1M
find . -type f -mtime -1
find build -name "*.o" -delete          # or: -exec rm {} \;
find . -name "*.c" -exec wc -l {} +     # line counts of every .c file
```

### Drill 08

Use `awk` to print field 1 (the user name) of `/etc/passwd`. Print the user name and shell (field 7) of users with UID ≥ 1000.

```sh
awk -F: '{print $1}' /etc/passwd
awk -F: '$3 >= 1000 {print $1, $7}' /etc/passwd
ps aux | awk '{print $2, $11}'      # PID and command
```
`-F:` sets the separator; `$0` is the whole line, `NR` the line number, `NF` the number of fields.

### Drill 09

Count how many users use each shell (field 7 of `/etc/passwd`), sorted in descending order.

```sh
cut -d: -f7 /etc/passwd | sort | uniq -c | sort -nr
#     20 /usr/sbin/nologin
#      3 /bin/bash
#      1 /bin/sync
```
`uniq` only merges **adjacent** lines → `sort` first. `sort -n` numeric, `-r` reverse, `-k2` by field 2, `-u` unique.

### Drill 10

Count the lines, words and bytes of `/etc/passwd`. Count the `.c` files in `bosch-lab`.

```sh
wc /etc/passwd          # lines  words  bytes  name
wc -l /etc/passwd
find . -name "*.c" | wc -l
ls | wc -l
```

### Drill 11

Print the first 5 lines, the last 5 lines, and everything from line 2 onwards of `/etc/passwd`. Follow a log in real time.

```sh
head -n 5 /etc/passwd
tail -n 5 /etc/passwd
tail -n +2 /etc/passwd          # skip the first line (a header)
sed -n '3,6p' /etc/passwd       # lines 3 to 6
sudo tail -f /var/log/syslog    # Ctrl+C to quit
```

### Drill 12

Find the running `bash` processes with `ps aux | grep` and with `pgrep`. What do the `VSZ`, `RSS` and `STAT` columns of `ps aux` mean?

```sh
ps aux | grep bash              # the "grep bash" line shows up too!
ps aux | grep [b]ash            # trick to exclude the grep itself
pgrep -a bash
ps -ef --forest                 # process tree (UID PID PPID ...)
ps -o pid,ppid,stat,ni,cmd -p $$
```
VSZ: virtual memory (KB). RSS: physical memory actually in RAM. STAT: R/S/D/T/Z, `+` foreground, `s` session leader, `<` high priority, `N` low priority.

### Drill 13

Open `top`: sort by memory, by CPU, show each core, quit. What do the 3 numbers of `load average` mean?

```sh
top            # M: by memory, P: by CPU, 1: per core, k: kill, q: quit
htop           # a nicer version (sudo apt install htop)
uptime         # load average: 0.52, 0.40, 0.35
nproc          # number of cores, to compare with the load
```
Load average = the average number of processes running or waiting for the CPU (and waiting for I/O in state D) over 1, 5 and 15 minutes.

### Drill 14

Run `sleep 100`, press `Ctrl+Z`, continue it in the background, list the jobs, bring it back to the foreground, then end it. Start a command directly in the background.

```sh
sleep 100
^Z                  # [1]+  Stopped   sleep 100   (sends SIGTSTP)
bg %1               # continue in the background (sends SIGCONT)
jobs -l             # [1]+ 12345 Running   sleep 100 &
fg %1               # back to the foreground
^C                  # SIGINT
sleep 200 &         # starts in the background; $! is its PID
kill %1             # kill by job number
nohup ./server &    # keeps running after the terminal closes (ignores SIGHUP)
```

### Drill 15

List the signal names. How do `kill PID`, `kill -9 PID`, `pkill` and `killall` differ?

```sh
kill -l                 # list of signals
kill 1234               # sends SIGTERM (15) - can be caught/ignored
kill -9 1234            # SIGKILL - cannot be caught
kill -SIGSTOP 1234; kill -CONT 1234
pkill -f "python app"   # by pattern (-f matches the whole command line)
killall sleep           # by exact name
```

### Drill 16

Compare `df -h` and `du -sh`. Which directory under `/var` takes the most space?

```sh
df -h                   # free space of each mounted FILESYSTEM
du -sh ~/bosch-lab      # total size of one DIRECTORY
sudo du -sh /var/* 2>/dev/null | sort -h | tail -3
df -i                   # free inodes
lsblk                   # disks / partitions
```
The two can disagree: a deleted file still held open by a process is counted by `df` but not by `du`.

### Drill 17

Run `make run T=03-process/05_zombie` in terminal 1. In terminal 2, look at the files that process has open using `lsof` and `/proc`. Find which process listens on port 22.

```sh
pgrep 05_zombie
lsof -p <PID>               # cwd, txt (the program), mem (.so libraries), 0u 1u 2u (/dev/pts/N)
ls -l /proc/<PID>/fd        # 0 -> /dev/pts/1, 1 -> /dev/pts/1, 2 -> /dev/pts/1
cat /proc/<PID>/status | head
sudo lsof -i :22            # the process listening on port 22
ss -tlnp                    # listening TCP ports
```

### Drill 18

Use `strace` to look at the system calls of `01_fork_count`: get call counts, and show only `clone`/`write`. Use `ltrace` for library calls.

```sh
strace -c ./build/03-process/01_fork_count          # syscall statistics table
strace -f -e trace=clone,write ./build/03-process/01_fork_count
strace -p <PID>                                     # attach to a running process
ltrace ./build/01-c/07_signed_unsigned              # printf, strlen...
```
glibc implements `fork()` with the `clone` system call (you will see `clone(child_stack=NULL, flags=...SIGCHLD...)`). `-f` also follows child processes.

### Drill 19

Print the exit code of the previous command, the PID of the current shell and the PID of the most recent background job. Add `~/bin` to PATH. How do shell variables and environment variables differ?

```sh
ls /does_not_exist; echo $?   # 2 (non-zero = error)
true; echo $?               # 0
echo $$                     # PID of the shell
sleep 50 & echo $!          # PID of the background job just started
export PATH="$HOME/bin:$PATH"
X=1; bash -c 'echo "X=$X"'          # X=   (shell variables are not passed to child processes)
export X; bash -c 'echo "X=$X"'     # X=1
cmd1 && cmd2    # cmd2 runs if cmd1 succeeds;   cmd1 || cmd2  if it fails
```
In a script: `$0` the script name, `$1..$9` arguments, `$#` the argument count, `$@` all arguments.

### Drill 20

Watch the 4 compilation stages of `01-c/08_macro_traps.c`. Look at the shared libraries the executable uses and the symbols in the `.o` file.

```sh
gcc -E 01-c/08_macro_traps.c | tail -20    # 1. preprocess: the macros are expanded
gcc -S 01-c/08_macro_traps.c -o m.s        # 2. compile to assembly
gcc -c 01-c/08_macro_traps.c -o m.o        # 3. assemble to an object file
gcc m.o -o m                               # 4. link
file m                 # ELF 64-bit LSB pie executable, x86-64, dynamically linked
ldd m                  # libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6
nm m.o                 # T main  (T: in text), U printf (U: undefined)
objdump -d m.o | head
```
Static libraries `.a` (copied into the executable at link time, `ar rcs`), shared libraries `.so` (loaded at run time, need `-fPIC -shared`, found via `LD_LIBRARY_PATH`/`ldconfig`).

## 10-alp

Labs following the book *Advanced Linux Programming* (Mitchell, Oldham, Samuel, 2001), chapters 1–5. Function names and examples follow the book's listings.

### 01_getopt_long — getopt_long: short/long options, optarg, optind, "--" and argv permutation

**Question:** For each sample argv, which options does getopt_long return, what is optarg, and where does optind stop? How is "-vo x.txt" understood? After "--", is "-h" still an option? What happens with "-x" (not declared) and "-o" without its argument?

**Expected output:**

```text
Sample 1: -v -o out.txt in1 in2
  -v/--verbose
  -o/--output, optarg = out.txt
  optind = 4, remaining arguments: in1 in2
Sample 2: --output=log.txt --verbose file
  -o/--output, optarg = log.txt
  -v/--verbose
  optind = 3, remaining arguments: file
Sample 3: -vo x.txt -- -h
  -v/--verbose
  -o/--output, optarg = x.txt
  optind = 4, remaining arguments: -h
Sample 4: in1 -v in2 --output o.txt
  -v/--verbose
  -o/--output, optarg = o.txt
  optind = 4, remaining arguments: in1 in2
  argv after permutation: -v --output o.txt in1 in2
Sample 5: -x -o
prog: invalid option -- 'x'
  '?' (optopt = 'x')
prog: option requires an argument -- 'o'
  '?' (optopt = 'o')
  optind = 3, remaining arguments:
```

**Explanation:**

- `"ho:v"`: `o` has a `:` so it needs an argument, either the next word (`-o out.txt`) or attached (`-oout.txt`). Long options: `--output=log.txt` or `--output log.txt`.
- `-vo x.txt`: combined short options; `v` takes no argument, `o` takes the next word.
- `--` ends the options: everything after it (even `-h`) is a plain argument.
- GNU getopt **permutes** argv: the plain arguments (`in1`, `in2`) are moved to the end, so after the loop you only need to walk from `optind` to `argc`.
- An undeclared option or a missing argument → `'?'` is returned, the offending character is in `optopt`, and an error message goes to **stderr** (turn it off with `opterr = 0`). Listing 2.2 then calls `print_usage(stderr, 1)`.

**Exam relevance:** GNU conventions: short option `-h`, long option `--help`; every program should support `--help` and `--version`. The `struct option` array must end with `{NULL, 0, NULL, 0}`. The `has_arg` field is 0 (none), 1 (required) or 2 (optional).

### 02_env_exit_code — Environment variables and exit codes, including death by signal

**Question:** Does setenv(..., 0) overwrite? Do the child process and a program it execs see the variable just set? What WEXITSTATUS does system("exit 300") give? When the shell is killed by SIGTERM, what is the wait status, and what $? does the parent shell see?

**Expected output:**

```text
environ has > 0 variables, PATH is set
SERVER_NAME = (NULL) -> using the default server.my-company.com
after setenv: SERVER_NAME = backup.example.net
child process sees: backup.example.net
program after exec sees: backup.example.net
after unsetenv: NULL
system("exit 3"): WIFEXITED, WEXITSTATUS = 3
system("exit 300"): WIFEXITED, WEXITSTATUS = 44
system("kill -TERM $$"): WIFSIGNALED, WTERMSIG = 15 (Terminated)
Terminated
  the shell sees $? = 143
system("sh -c 'kill -TERM $$'; echo "  the shell sees \$? = $?"; exit 0"): WIFEXITED, WEXITSTATUS = 0
system("no_such_command 2>/dev/null"): WIFEXITED, WEXITSTATUS = 127
```

**Explanation:**

- `setenv(name, value, 0)`: does not overwrite an existing variable.
- The environment is copied to the child at fork and kept across exec (exec without the `e` suffix).
- `getenv` returns NULL when the variable is missing → the program uses a default (listing 2.4).
- Only the low 8 bits of the exit code survive: 300 mod 256 = 44. That is why the book recommends 0–127 only.
- A shell killed by SIGTERM: the wait status says "killed by signal 15", with no exit code. The parent shell then reports `$?` = 128 + 15 = 143 and prints "Terminated".
- Command not found: the shell returns 127 (126 if the file exists but cannot be executed).

**Exam relevance:** `printenv`, `export VAR=value`, `echo $?`. The `environ` variable (`extern char **environ;`) is a NULL-terminated array of `"NAME=value"` strings; do not modify it directly — use `setenv`/`unsetenv`. A child process can never change its parent's environment.

### 03_stdio_buffer — stdio buffering: full, line, unbuffered; exit vs _exit

**Question:** With stdout set to full buffering, in what order do the [stdout] and [stderr] parts appear? In the second part, in what order do A/B/C/D/E appear? Does the last line appear?

**Expected output:**

```text
[stderr 2] [stderr 4]
[stdout 1] [stdout 3]
C(none) B(line) D(line)
E(none)
A(full)
```

**Explanation:**

- stdout is fully buffered: `[stdout 1]` and `[stdout 3]` stay in the buffer until `fflush`. stderr is unbuffered, so it comes out first.
- Second part: `C` (unbuffered) comes out immediately; `B` stays in the line-buffered stream until the `'\n'` of `D`; `E` comes out immediately; `A` (fully buffered) only comes out at `fclose`.
- The last line is in stdout's buffer; `_exit` does not flush stdio, so it is lost. `exit()` (or returning from main) would flush it.

**Exam relevance:** stdout to a terminal is line-buffered, to a file/pipe fully buffered (so `./app | cat` can change the order relative to stderr). The book's example: `while (1) { printf("."); sleep(1); }` does not print one dot per second; switching to stderr does. Merging both streams: `prog > out.txt 2>&1` (2>&1 **after** a file redirection) and `prog 2>&1 | filter` (**before** a pipe).

### 04_tempfile — Safe temporary files: mkstemp + immediate unlink, tmpfile

**Question:** What permissions does mkstemp give the file? After unlink, does access() still see the file, and can you still read through the fd? What is st_nlink before and after unlink?

**Expected output:**

```text
file name: /tmp/alp_temp.<6 chars>, still XXXXXX ? no, length = 20
mode = 600, st_nlink = 1
after unlink: access() = -1 (No such file or directory), st_nlink = 0
read back through the fd: length = 15, data = "temporary data"
tmpfile: read back 42 tmpfile, fileno(f) > 2 ? 1
```

**Explanation:**

- The name template must end with exactly 6 `X`s; mkstemp modifies the array in place (so it must be a `char[]`, not a string literal).
- Mode 0600: only the owner can read/write.
- `unlink` removes the name (the link count drops to 0) but the inode lives on because the fd is still open → it can still be read and written. Only when the fd is closed or the process dies is the file really deleted, so no junk is left in /tmp.
- Listing 2.5 writes the length first, then the data, `lseek`s back to the start and reads it back.
- `tmpfile()` returns a `FILE*` that is already unlinked.

**Exam relevance:** Three requirements for temporary files: no name collisions between instances, permissions that stop others from tampering, unpredictable names (no race between checking and opening). Use mkstemp (an fd, the name can be handed to another program) or tmpfile (a FILE*, internal use only). Do not use mktemp, tmpnam, tempnam.

### 05_errno_assert — errno, strerror, perror and assert ⚠️ dies on purpose with SIGABRT

**Question:** What is errno after each failing call? After a SUCCESSFUL call, does errno go back to 0? With NDEBUG, how many times does assert(do_something() == 0) call the function? How does the last line end the program?

**Expected output:**

```text
open("/does/not/exist")      -> -1, errno =  2 ENOENT  (No such file or directory)
getpid() (succeeds)          ->  1, errno =  2 ENOENT  (No such file or directory)
open("/tmp", O_WRONLY)       -> -1, errno = 21 EISDIR  (Is a directory)
close(12345)                 -> -1, errno =  9 EBADF   (Bad file descriptor)
mkdir("/tmp")                -> -1, errno = 17 EEXIST  (File exists)
lseek(0, 0, 12345)           -> -1, errno = 22 EINVAL  (Invalid argument)
open("/etc/shadow")          -> -1, errno = 13 EACCES  (Permission denied)
perror prints to stderr: No such file or directory
NDEBUG: calls = 0
without NDEBUG: calls = 3
05_errno_assert: 10-alp/05_errno_assert.c:64: main: Assertion `pointer != NULL' failed.
```

**Explanation:**

- A failing system call returns -1 and sets errno. A successful call does **not** reset errno → after getpid errno is still ENOENT from before.
- Save errno immediately (`int saved = errno;`), because printf or any other call may change it.
- `perror("msg")` prints `msg: <strerror(errno)>` to stderr.
- With NDEBUG the preprocessor removes the asserts → `do_something()` is never called. That is why assert expressions must have no side effects (the book: store the result in a variable first, then `assert(status == 0)`).
- A false assert prints the program name, file, line, function and expression, then calls `abort()` → SIGABRT. The shell reports `Aborted (core dumped)` and exit code 134 = 128 + 6.

**Exam relevance:** Common error codes: EPERM, ENOENT, EINTR (interrupted by a signal → retry), EBADF, EACCES, EEXIST, ENOTDIR, EISDIR, EINVAL, ENOMEM, EAGAIN. Use assert for **internal** checks (NULL pointers, parameter conditions), never to validate user input.

### 06_dlopen — Loading a shared library at run time: dlopen, dlsym, dlerror, dlclose

**Question:** Can cos be called through the pointer returned by dlsym? What do you get from dlsym on a missing name, or dlopen on a missing library? What does dlerror() return when called twice in a row?

**Expected output:**

```text
cos(0) = 1.0, sqrt(2) = 1.4142
dlsym of a missing name: (nil)
dlerror() 1st call: /lib/x86_64-linux-gnu/libm.so.6: undefined symbol: no_such_function
dlerror() 2nd call: (NULL)
dlopen of a missing library: (nil), dlerror: libnosuchlib.so: cannot open shared object file: No such file or directory
dlclose = 0
```

**Explanation:**

- dlsym returns a `void*`; cast it to the right function pointer type and call it.
- Errors are reported as NULL; the details come from `dlerror()`. Once read, the error is cleared, so the second call returns NULL.
- dlopen searches LD_LIBRARY_PATH, the ldconfig cache, /lib and /usr/lib.

**Exam relevance:** Used for plugins. `RTLD_LAZY` (resolve each symbol on first call) vs `RTLD_NOW` (resolve everything up front). Functions in a C++ library must be `extern "C"`. Link with `-ldl` (merged into libc since glibc 2.34). dlopen'ing the same library several times only increments a reference count.

### 07_fork_exec — system, fork + exec, spawn, nice

**Question:** Is the line after exec printed? When execvp runs a command that does not exist, what does the child do and what status does the parent see? Is HOME set in the program run by execle? Does nice(10) in the child change the parent's priority?

**Expected output:**

```text
1) system:
  [sh] $0 = sh
  system returned 0
2) spawn("echo", ...) with fork + execvp:
  [echo] hello from the new program
  [parent] child exited normally, exit code 0
3) spawn a command that does not exist:
  [child] execvp error: No such file or directory
  [parent] child killed by signal 6 (Aborted)
4) execl (full path) and the line after exec:
  [echo] run with execl
  [parent] child exited normally, exit code 0
5) execle with a custom environment:
  [sh] GREETING=hello, HOME=(not set)
  [parent] child exited normally, exit code 0
6) nice:
  [child] nice: 0 -> 10
  [parent] nice is still 0
```

**Explanation:**

- `system()` runs the command through `/bin/sh -c` → `$0` is `sh`.
- A successful exec never returns; the code after it only runs when exec fails. In listing 3.4 the child calls `abort()` after a failed exec → the parent sees SIGABRT.
- execle uses exactly the environment array it is given: only GREETING, no HOME.
- nice is a per-process attribute; the child raising its niceness (lowering its priority) does not affect the parent. Niceness ranges from -20 (highest priority) to 19; only root can lower it.

**Exam relevance:** The exec family: `execl`, `execlp`, `execle`, `execv`, `execvp`, `execve` (the real system call). `p`: search PATH; `v`: argv as an array; `l`: arguments listed and terminated by `(char *)NULL`; `e`: pass an environment. The book recommends fork + exec over system because system depends on the shell, has security holes and is slow.

### 08_wait_status — wait/waitpid, the status macros, WNOHANG, zombies

**Question:** What WEXITSTATUS does exit(300) give? What do WIFSIGNALED/WTERMSIG report for abort() and kill -9? What does waitpid(..., WNOHANG) return while the child is still running? What state does /proc show for a child that exited but was not waited for? What about wait() when there are no children?

**Expected output:**

```text
child exit(300):       WIFEXITED   WEXITSTATUS = 44
child exit(-1):        WIFEXITED   WEXITSTATUS = 255
child abort():         WIFSIGNALED WTERMSIG = 6 (Aborted)
child kill -9:         WIFSIGNALED WTERMSIG = 9 (Killed)
WNOHANG while the child runs: waitpid = 0
child exited, not waited for: state in /proc = Z
WNOHANG 2nd call: returned the child's PID ? 1
child exit(7):         WIFEXITED   WEXITSTATUS = 7
after wait: /proc/<pid> = - (gone)
wait() with no children left: -1, errno = ECHILD
```

**Explanation:**

- Only the low 8 bits survive: 300 → 44, -1 → 255.
- A process killed by a signal has no exit code; use `WTERMSIG` to find out which signal.
- `WNOHANG`: returns 0 if no child has finished yet, without blocking (for periodic checks).
- A child that exited without being waited for → a **zombie** (`Z` in ps//proc): it only keeps its PID and exit status. After wait the entry disappears completely.
- No children left → wait returns -1 with errno = ECHILD.

**Exam relevance:** `wait` blocks until any child ends; `waitpid(pid, &status, options)` waits for a specific child (pid = -1 means any); `wait3`/`wait4` also return resource usage. If the parent dies first, the zombie child is adopted and reaped by init. Automatic reaping: a SIGCHLD handler calling wait (listing 3.7), or setting SIGCHLD to SIG_IGN.

### 09_thread_return — pthreads: passing data, return values, pthread_exit, detached threads ⚠️ nondeterministic

**Question:** What is the 1000th prime computed by the thread? What does pthread_equal(self, main) give inside the child thread? What happens when you pthread_join a detached thread? Is it safe for two character-printing threads to use structs on main's stack?

**Expected output:**

```text
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxoooooooooooooooooooo     <- 30 x and 20 o; the interleaving may differ
(30 x and 20 o; the interleaving is not fixed)
prime number 1000 = 7919
in the child thread: pthread_equal(self, main) = 0
pthread_exit((void*)7) -> join got 7
pthread_join on a detached thread: 22 (EINVAL)
```

**Explanation:**

- Listings 4.2/4.3: each thread gets its own struct through `void*`. It is safe because main joins both threads before the structs (on main's stack) go away. If main returned first, the threads would read destroyed memory (the bug the book shows in listing 4.2).
- Listing 4.4: the return value is cast to `void*` and recovered through the second argument of `pthread_join`.
- `pthread_exit(v)` is equivalent to `return v` from the thread function.
- Compare thread IDs with `pthread_equal`, not `==` (pthread_t may be a struct).
- A detached thread cannot be joined: glibc returns EINVAL (POSIX calls it undefined behavior).

**Exam relevance:** Compile with `-pthread` (the book uses `-lpthread`). pthread functions return an **error code** instead of setting errno. A joinable thread nobody joins stays a "zombie thread" until it is joined; threads whose result you do not need should be detached (`pthread_detach` or the `PTHREAD_CREATE_DETACHED` attribute).

### 10_thread_cancel — Thread cancellation: deferred, uncancelable sections, cleanup handlers

**Question:** Thread A is cancelled while sleeping: does its cleanup handler run, and what does join return? Thread B disables cancellation during a "money transfer": where is it cancelled, and is "transfer done" printed? What does pthread_cleanup_pop(1) do when the thread is not cancelled?

**Expected output:**

```text
Thread A:
  [cleanup] free buffer
  join A: PTHREAD_CANCELED
Thread B:
  [B] transfer started (cancellation disabled)
  [main] pthread_cancel(B) = 0
  [B] transfer done
  join B: PTHREAD_CANCELED
Thread C:
  [C] finished normally, pop(1) still runs the handler:
  [cleanup] free buffer
  join C: 42
```

**Explanation:**

- A is deferred cancelable (the default): it is cancelled at `usleep` (a cancellation point) → the cleanup handler runs and frees the buffer → join gets `PTHREAD_CANCELED`.
- B disables cancellation: the request is **held**; B finishes its work, re-enables cancellation and is cancelled at `pthread_testcancel()` → "never reached" is not printed. That is the idea of listing 4.6 (a transfer between two accounts must not stop halfway).
- `pthread_cleanup_pop(1)` removes the handler and runs it; `pop(0)` only removes it. push/pop must be paired in the same block (they are macros that open/close `{ }`).

**Exam relevance:** Three states: asynchronously cancelable (cancelled at any time, dangerous), synchronously/deferred cancelable (the default, cancelled at cancellation points), uncancelable. Functions: `pthread_cancel`, `pthread_setcanceltype`, `pthread_setcancelstate`, `pthread_testcancel`. In C++ the book recommends throwing an exception to leave a thread, so destructors of local objects run.

### 11_tsd_mutex — Thread-specific data and kinds of mutex, semaphores

**Question:** Whose value does each thread read with pthread_getspecific? When does the key's destructor run? What does a second lock return for an error-checking mutex, and for a recursive one? Another thread unlocking an error-checking mutex? trylock on a mutex someone holds? sem_trywait when the value is 0?

**Expected output:**

```text
Thread-specific data:
  thread1.log -> writing "Thread starting."
  [destructor] closing "thread1.log"
  thread2.log -> writing "Thread starting."
  [destructor] closing "thread2.log"
  thread3.log -> writing "Thread starting."
  [destructor] closing "thread3.log"
  main: pthread_getspecific = NULL
Mutex:
  errorcheck 1st lock: 0
  errorcheck 2nd lock: EDEADLK
  another thread unlocks errorcheck: EPERM
  errorcheck unlock (by the owner): 0
  recursive lock, lock: 0 0; unlock x3: 0 0 EPERM
  fast: trylock in the thread holding it: EBUSY
  another thread trylocks the held mutex: EBUSY
Semaphore:
  initialized to 2, sem_getvalue = 2
  after 2 sem_waits: sem_trywait = -1, errno = EAGAIN
  after sem_post: 1
```

**Explanation:**

- Listing 4.7: one key, `thread_log_key`, and each thread attaches its own log file name. `write_to_thread_log` takes no parameter yet writes to the calling thread's file. main never set anything, so it gets NULL.
- The key's destructor runs automatically when a thread ends (for non-NULL values), like listing 4.7 closing the log file.
- Error-checking: relocking in the same thread → EDEADLK instead of hanging; unlocking from a non-owner → EPERM.
- Recursive: locked 2 times means unlocked 2 times; a 3rd unlock → EPERM.
- trylock never blocks: a held mutex (even held by yourself) → EBUSY.
- Counting semaphore: two waits bring it to 0; `sem_trywait` does not block but returns -1 with errno = EAGAIN (semaphores use errno, unlike mutexes).

**Exam relevance:** The book's three kinds of mutex: fast (the default; relocking deadlocks), recursive, error-checking. `PTHREAD_MUTEX_INITIALIZER` for static initialization. Thread semaphores: `sem_init(&s, 0, value)` (0 = shared only between the process's threads), `sem_wait`, `sem_post`, `sem_trywait`, `sem_getvalue`, `sem_destroy`. The book's job queue uses a mutex + a semaphore counting the jobs.

### 12_sysv_shm_sem — System V shared memory and semaphores

**Question:** What is shm_nattch before fork, while the child is alive, and after the child exits? Can the parent read the string the child wrote into shared memory? semop -1 with IPC_NOWAIT when the value is 0? The child adds to the semaphore with SEM_UNDO and exits: what value is left?

**Expected output:**

```text
segment: size = 4096 (= getpagesize()), nattch = 1
semop -1 IPC_NOWAIT at 0: -1, errno = EAGAIN
  [child] wrote it, nattch = 2
  [parent] read: "Hello, world (from the child)"
after the child exited: nattch = 1
  [child] +3 with SEM_UNDO, value = 3
child exited, the kernel undid it: value = 0
after the parent's shmdt: nattch = 0
after IPC_RMID: shmctl IPC_STAT -> EINVAL/EIDRM (removed)
```

**Explanation:**

- A segment's size should be a multiple of the page size (`getpagesize()`, usually 4096).
- fork makes the child inherit the attached region → nattch = 2; an exiting process detaches automatically.
- The child writes and the parent reads it immediately: the same physical memory, no copying. The semaphore makes sure the parent only reads after the child has written.
- `IPC_NOWAIT`: semop does not block but returns EAGAIN.
- `SEM_UNDO`: the kernel remembers the change and undoes it when the process exits, so the semaphore does not get stuck if a process dies halfway.
- The segment only disappears after `IPC_RMID` (and once nattch reaches 0).

**Exam relevance:** Commands: `ipcs -m` (shm), `ipcs -s` (semaphores), `ipcrm`. Shared memory is the **fastest** IPC because reads/writes do not go through the kernel, but you must synchronize yourself. `shmget(key, size, IPC_CREAT | IPC_EXCL | perms)`, `shmat(id, NULL, 0)`, `shmdt(addr)`, `shmctl(id, IPC_RMID, NULL)`. semop takes an array of `struct sembuf {sem_num, sem_op, sem_flg}`; semctl needs a hand-declared `union semun`.

### 13_mmap_shared_private — mmap: MAP_SHARED, MAP_PRIVATE, msync, sharing through fork

**Question:** Does writing to the MAP_PRIVATE region change the file? What does the MAP_SHARED region show? After writing through MAP_SHARED, what does pread of the file show? The child increments a counter 1000 times in a MAP_SHARED|MAP_ANONYMOUS region: what does the parent see, and with MAP_PRIVATE?

**Expected output:**

```text
at first: shared = "original", private = "original"
write via MAP_PRIVATE: private = "via PRIVATE", shared = "original", file = "original"
msync = 0
write via MAP_SHARED: shared = "via SHARED", file = "via SHARED", private = "via PRIVATE"
after the child incremented 1000 times: MAP_SHARED = 1000, MAP_PRIVATE = 0
```

**Explanation:**

- MAP_PRIVATE is copy-on-write: the first write creates a private copy of the page; the file and other mappings do not see it.
- MAP_SHARED writes straight into the file's page cache → `pread` sees it immediately. `msync(MS_SYNC)` waits until it is on disk.
- The private page has already been copied, so it no longer sees changes to the file.
- An anonymous shared mapping is inherited across fork and shared → the parent sees 1000. A private mapping is copy-on-write after fork → the parent still sees 0.

**Exam relevance:** `mmap(addr, length, prot, flags, fd, offset)`; prot: `PROT_READ`/`PROT_WRITE`/`PROT_EXEC`; flags: `MAP_SHARED`/`MAP_PRIVATE` (pick one), `MAP_FIXED`, `MAP_ANONYMOUS`. Errors return `MAP_FAILED`, not NULL. The file must be long enough before mapping (the book uses lseek + writing one byte at the end). Accessing past the end of the file → SIGBUS. msync flags: `MS_ASYNC`, `MS_SYNC`, `MS_INVALIDATE`.

### 14_popen_fdopen — Pipes with FILE*: fdopen, dup2 + exec sort, popen/pclose

**Question:** When do the lines sent to sort appear, and in what order? What status does pclose return for popen("...; exit 3", "r")? When does the child's fgets loop on the pipe stop?

**Expected output:**

```text
1) pipe + fdopen:
  [child] got: Hello, world #1
  [child] got: Hello, world #2
  [child] got: Hello, world #3
  [child] fgets returned NULL: EOF because the parent closed the write end
2) dup2 + execlp("sort"):
  [parent] wrote all 5 lines; sort prints nothing yet because it has not seen EOF
  Hello, world.
  My dog has fleas.
  One fish, two fish.
  This is a test.
  This program is great.
3) popen("sort", "w"):
  apple
  banana
  cherry
  pclose = 0 (WEXITSTATUS 0)
4) popen(..., "r"):
  read "42", pclose -> WEXITSTATUS 3
```

**Explanation:**

- fgets returns NULL once every write end is closed (the child closed its own `fds[1]`, the parent fclose'd its copy).
- sort must read all of its input (until EOF) before it can print → it only prints after the parent's `close(fds[1])`.
- popen with "w": we write to the command's stdin; `pclose` closes the pipe (sending EOF), waits for the child and returns its wait status.
- popen with "r": we read the command's stdout; the command's exit code comes from `WEXITSTATUS(pclose(...))`.

**Exam relevance:** A pipe is one-way, FIFO-ordered, and only usable between related processes (named FIFOs from `mkfifo` lift that restriction). Writing to a pipe with no reader → SIGPIPE/EPIPE. Reading an empty pipe while a write end is open → blocks. popen carries the same security risks as system because it goes through the shell.

### 15_sockets — socketpair, an Internet socket over 127.0.0.1, byte order

**Question:** Is a socketpair one-way or two-way? What is htons(80) on an x86 machine (in hex)? Which port does a server get when it binds port 0? When does read return 0?

**Expected output:**

```text
htons(80) = 0x5000, htonl(0x7f000001) = 0x0100007f, ntohs(htons(8080)) = 8080
  [child] got "ping", replying pong
  [parent] got "pong" on the same socket (two-way)
  [parent] read after the child closed its socket = 0 (EOF)
server listening on 127.0.0.1, port > 1023 ? 1
  [server] accept returned a new socket (different from the listening one ? 1), got: GET /
  [client] got: HTTP/1.0 200 OK
```

**Explanation:**

- x86 is little-endian, network byte order is big-endian → htons swaps the two bytes: 80 = 0x0050 → 0x5000.
- A socketpair is two-way: the same fd both writes and reads (unlike a pipe).
- When the other side closes the connection → read returns 0 (EOF).
- Port 0 → the kernel assigns a free ephemeral port; read it back with `getsockname`.
- `accept` returns a **new** socket for this connection; the original socket keeps listening for other clients.

**Exam relevance:** The three socket parameters: namespace (`PF_LOCAL`/`PF_UNIX`, `PF_INET`), style (`SOCK_STREAM`: connected, ordered, reliable; `SOCK_DGRAM`: datagrams that may be lost/reordered), protocol (0 = the default). Local sockets use a file path (`sockaddr_un.sun_path`) and must be `unlink`ed afterwards. Ports below 1024 need root. The book uses `gethostbyname` to resolve names (today use `getaddrinfo`).

## 11-design-patterns

Design patterns in C++, following the list of patterns covered by the tutorialspoint "Design Patterns" tutorial (GoF creational, structural and behavioral patterns, plus the J2EE patterns). The code and explanations are original.

### 01_singleton — Singleton: lazy Meyers singleton, deleted copy, thread-safe initialization

**Question:** Is "Config() constructed" printed before or after "main starts"? How many times is it printed when 4 threads call instance() at once? Are a and b the same object? Does "Eager() constructed" appear before main?

**Expected output:**

```text
Eager() constructed
main starts
Config() constructed
same object? 1, b.get("mode") = release
main ends
~Config()
```

**Explanation:**

- `Eager::inst` is a static member object → it is constructed during static initialization, **before** `main`.
- `Config::instance()` holds a function-local `static` → it is constructed on the **first call** (inside one of the threads), after "main starts".
- Since C++11 the initialization of a function-local static is thread-safe: if 4 threads race into `instance()`, exactly one constructs it and the others wait → printed **once**.
- Every call returns a reference to the same object → `&a == &b`; a change through `a` is visible through `b`.
- The private constructor and the deleted copy constructor/assignment make a second instance impossible. The static object is destroyed after `main` returns (`~Config()`).

**Exam relevance:** Singleton = one instance + a global access point. Typical questions: why the constructor is private, why copy is deleted, lazy vs eager initialization, thread safety (the old `if (!p) p = new T;` is a data race; double-checked locking needs atomics; the Meyers singleton is the simple answer). Drawbacks: hidden global state, harder unit testing.

### 02_factory — Factory, Factory Method and Abstract Factory

**Question:** What does ShapeFactory::create return for "SQUARE" and for "TRIANGLE"? Which button does each Dialog create? Which family of widgets does each GUI factory produce, and can a Windows button be mixed with a Linux checkbox?

**Expected output:**

```text
Simple factory:
  Circle::draw
  Square::draw
  TRIANGLE -> nullptr
Factory method:
  dialog shows <button>OK</button>
  dialog shows [ OK ]
Abstract factory:
  [ OK ] + [x] (Windows)
  ( OK ) (GTK) + (x) (GTK)
```

**Explanation:**

- Simple factory: the caller passes a string and gets a `unique_ptr<Shape>`; it never names `Circle` or `Square`. An unknown type returns `nullptr`.
- Factory Method: `Dialog::show()` is written once and calls the virtual `createButton()`; each subclass decides which concrete button to create.
- Abstract Factory: `WinFactory` only creates Windows widgets and `LinuxFactory` only GTK widgets, so a family is always consistent; `buildUi` works with any factory through the `GuiFactory` interface.
- Returning `unique_ptr` makes ownership explicit and avoids leaks.

**Exam relevance:** Factory hides `new` and the concrete class; Factory Method uses inheritance (a subclass decides); Abstract Factory uses composition and creates **families** of related objects. Adding a new product family to an abstract factory is easy (a new factory class); adding a new product kind is hard (every factory changes).

### 03_builder_prototype — Builder and Prototype

**Question:** Which items and total cost does each meal have? What does the fluent HttpRequest builder produce? After cloning the prototype and changing the clone's tags, does the original change? With the shallow clone, does it?

**Expected output:**

```text
Veg meal:
  - Veg burger 25
  - Coke 30
  total 55
Non-veg meal:
  - Chicken burger 50.5
  - Pepsi 35
  total 85.5
request: POST /api/runs, 1 header(s), body {}
  original: Q3 report [draft ]
  clone   : Q3 report [draft final ]
shallow copy: a.pages = 99, same pointer? 1
```

**Explanation:**

- The meal builder assembles a `Meal` step by step; the same construction steps produce different representations (veg / non-veg).
- The fluent builder returns `*this` from every setter, so calls chain; `build()` returns the finished object, and unset fields keep their defaults.
- `Report::clone()` uses the copy constructor; `std::vector` copies its elements, so the clone is a **deep** copy and the original keeps only "draft".
- `ShallowDoc` holds a raw pointer; the default copy copies the address, so both objects point to the same int → changing it through `b` changes `a`.

**Exam relevance:** Builder: many optional parameters / complex construction (avoids "telescoping constructors"). Prototype: create by cloning, via a virtual `clone()` (a "virtual constructor"); be careful with deep vs shallow copies.

### 04_adapter_bridge — Adapter and Bridge

**Question:** Which files can AudioPlayer play directly, which through the adapter, and which not at all? What does each bridged shape print? How many classes would you need for 3 shapes × 3 renderers with plain inheritance, and with Bridge?

**Expected output:**

```text
Adapter:
  built-in mp3: beyond the horizon.mp3
  mp4 player: alone.mp4
  vlc player: far far away.vlc
  invalid media: avi
Bridge:
  red circle r=10 at (100,100)
  green circle r=10 at (100,100)
```

**Explanation:**

- mp3 is played by `AudioPlayer` itself; mp4 and vlc go through `MediaAdapter`, which implements the `MediaPlayer` interface and forwards to an `AdvancedMediaPlayer`; avi is not supported.
- Bridge: `Circle` (the abstraction) holds a `DrawAPI` (the implementation); the same circle code draws red or green depending on the object passed in.
- With inheritance, 3 shapes × 3 renderers = 9 classes; with Bridge, 3 + 3 = 6, and both sides grow independently.

**Exam relevance:** Adapter makes **existing** incompatible interfaces work together (after the fact); Bridge is designed **up front** to separate abstraction and implementation. Object adapter (composition) vs class adapter (multiple inheritance).

### 05_composite_decorator — Composite and Decorator

**Question:** What is the total salary of the CEO's tree, and of the marketing head's subtree? In what order are the employees printed? What does each decorated coffee cost and print, and does the order of decorators change the description?

**Expected output:**

```text
Composite:
  John (CEO, 30000)
    Robert (Head Sales, 20000)
      Richard (Sales, 10000)
      Rob (Sales, 10000)
    Michel (Head Marketing, 20000)
      Laura (Marketing, 10000)
      Bob (Marketing, 10000)
  total salary: CEO tree = 110000, marketing subtree = 40000
Decorator:
  Espresso = 2
  Espresso + milk + sugar = 2.75
  Espresso + sugar + milk + milk = 3.25
```

**Explanation:**

- Composite: `totalSalary()` and `print()` are the same calls on a leaf and on a group; recursion walks the tree depth-first: 30000 + 2 × 20000 + 4 × 10000 = 110000; marketing = 20000 + 2 × 10000 = 40000.
- Decorator: each add-on wraps another `Beverage` and adds to its result. `Sugar(Milk(Espresso))` = 2 + 0.5 + 0.25; `Milk(Milk(Sugar(Espresso)))` = 2 + 0.25 + 0.5 + 0.5. The description follows the wrapping order (innermost first).

**Exam relevance:** Composite = part–whole hierarchies (file systems, GUI widgets, org charts), uniform treatment of leaves and composites. Decorator = adding responsibilities at run time without a subclass explosion (C++ streams, middleware); it keeps the same interface, unlike an adapter.

### 06_facade_proxy_flyweight — Facade, Proxy and Flyweight

**Question:** How many lines does computer.start() print? When is the image actually loaded from disk, and how many times for two display() calls? How many Circle objects does the flyweight factory create for 10 circles in 3 colors?

**Expected output:**

```text
Facade:
  cpu: freeze
  disk: read boot sector
  memory: load bootloader at 31744
  cpu: jump to 31744
  cpu: execute
Proxy:
  proxy created, nothing loaded yet
  loading photo.jpg from disk
  displaying photo.jpg
  displaying photo.jpg
Flyweight:
  10 circles drawn, Circle objects created = 3
```

**Explanation:**

- The facade's single `start()` drives three subsystems (5 steps); the client never touches `Cpu`, `Memory` or `Disk`.
- The proxy creates the `RealImage` only on the first `display()` (lazy loading) and reuses it → loaded once, displayed twice. 0x7c00 is printed in decimal (31744).
- The flyweight factory caches one `Circle` per color → only 3 objects for 10 draws; the position is passed in on each call.

**Exam relevance:** Facade = a simplified entry point to a subsystem. Proxy kinds: virtual (lazy), protection (access control), remote, caching, smart pointer. Flyweight = sharing intrinsic state to save memory (characters in a text editor, particles, game tiles).

### 07_observer — Observer (publish/subscribe), with detaching and weak_ptr observers

**Question:** Which observers print after setState(15)? After detaching the octal observer and calling setState(10)? The log observer is held through a weak_ptr: what happens after it is destroyed?

**Expected output:**

```text
setState(15):
  binary: 00001111
  octal:  17
  hex:    F
  log:    state is now 15
setState(10):
  binary: 00001010
  hex:    A
  log:    state is now 10
setState(3):
  binary: 00000011
  hex:    3
  (a weak observer is gone, removing it)
```

**Explanation:**

- Every attached observer is notified, in attach order.
- After `detach(&oct_)` the octal observer is no longer called.
- The subject stores the log observer as a `weak_ptr`; after `log.reset()` the object is gone, `lock()` returns an empty pointer and the subject removes the entry instead of calling a dead object.

**Exam relevance:** Observer = one-to-many notification, loose coupling (event systems, MVC views, signals/slots). Pitfalls: dangling observers (detach in the destructor, or use `weak_ptr`), modifying the list while iterating, notification order, observers triggering more updates.

### 08_strategy_state_template — Strategy, State and Template Method

**Question:** What does the context compute with each strategy? Which states does the turnstile go through for coin, push, push, coin, coin? In what order does Game::play() call the steps for Cricket and for Football?

**Expected output:**

```text
Strategy:
  add(10, 5) = 15
  subtract(10, 5) = 5
  multiply(10, 5) = 50
State:
  start: Locked
  coin: unlock
  push: pass through, lock again
  push while locked: blocked
  coin: unlock
  coin while unlocked: refund
  end: Unlocked
Template method:
  cricket initialized
  cricket started
  cricket finished
  football initialized
  football started
  game over
```

**Explanation:**

- Strategy: the calculator does not change; only the injected operation object does.
- State: each state class handles events and replaces the turnstile's state itself: Locked —coin→ Unlocked —push→ Locked —push (blocked)→ Locked —coin→ Unlocked —coin (refund)→ Unlocked.
- Template method: `play()` is non-virtual and fixes the order initialize → start → end; Football does not override `end()`, so the default hook prints "game over".

**Exam relevance:** Strategy vs State: the same structure (delegation to an interface), but with Strategy the **client** chooses the algorithm, while with State the **states** switch themselves. Template Method uses inheritance; Strategy uses composition.

### 09_command_memento_chain — Command (with undo), Memento and Chain of Responsibility

**Question:** What is the stock quantity after the queued orders run, and after undoing the last one? Which saved state does the editor return to after restore(0)? For each log level, which loggers in the chain write the message?

**Expected output:**

```text
Command:
  2 orders queued, qty still 10
  bought 5, qty = 15
  sold 3, qty = 12
  undo:
  bought 3, qty = 15
  final qty = 15
Memento:
  current:  State #1, #2, #3
  restore(0): State #1
  restore(1): State #1, #2
Chain of responsibility:
  INFO message:
    console: this is information
  DEBUG message:
    file:    this is debug info
    console: this is debug info
  ERROR message:
    error:   this is an error
    file:    this is an error
    console: this is an error
```

**Explanation:**

- Command: orders are objects; queuing them does nothing until `placeAll()`. 10 + 5 − 3 = 12; undoing the sell buys 3 back → 15.
- Memento: `Editor::Memento` is opaque (private data, friend `Editor`); the caretaker (the vector) stores snapshots without looking inside; restoring brings back exactly the saved text.
- Chain: every logger in the chain gets the message and writes it if the message level ≥ its own level: INFO → console only; DEBUG → file and console; ERROR → all three.

**Exam relevance:** Command enables undo/redo, queues, macros and logging of requests (GUI actions, job queues). Memento = undo/snapshots without breaking encapsulation. Chain of Responsibility = request handlers (logging levels, event bubbling, middleware); the sender does not know which handler acts.

### 10_iterator_visitor_mediator — Iterator, Visitor and Mediator

**Question:** What does the range-for over NameRepository print, and the explicit hasNext()/next() loop? What do the two visitors print, and what is the total price? Who receives each chat message, and does a user receive their own message?

**Expected output:**

```text
Iterator:
  range-for: Robert John Julie Lora
  hasNext/next: Robert John Julie Lora
Visitor:
  displaying keyboard (104 keys)
  displaying mouse
  displaying monitor (27")
  total price = 245
Mediator:
  John <- Robert: Hi John!
  Julie <- Robert: Hi John!
  Robert <- John: Hello Robert!
  Julie <- John: Hello Robert!
```

**Explanation:**

- Both iteration styles walk the same names in order; the repository never exposes its `vector`.
- Visitor: `p->accept(v)` calls `v.visit(*this)` with the static type of `*this`, which picks the right overload (double dispatch). The price visitor accumulates 30 + 15 + 200 = 245.
- Mediator: users only talk to the chat room, which forwards a message to everyone except the sender.

**Exam relevance:** Iterator = traversal without exposing the representation (STL iterators). Visitor = add operations without changing the element classes, but adding a new element type changes every visitor. Mediator reduces many-to-many dependencies (air-traffic control, chat rooms, GUI dialogs).

### 11_interpreter_filter_null — Interpreter, Filter (Criteria) and Null Object

**Question:** Which inputs do the rules "is male" (Robert OR John) and "is married woman" (Julie AND married) accept? Which persons does each criteria filter keep? What does the program print for the customers "Rob" and "Bob", and does it ever check for nullptr?

**Expected output:**

```text
Interpreter:
  John is male? 1
  Julie is male? 0
  "Married Julie" is a married woman? 1
  "Julie" is a married woman? 0
Filter:
  males: Robert John Mike Bobby
  females: Laura Diana
  single males: Robert Mike Bobby
  single or female: Robert Diana Mike Bobby Laura
Null object:
  Rob -> Rob
  Bob -> Not available in the customer database
  Julie -> Julie
  Laura -> Not available in the customer database
```

**Explanation:**

- Interpreter: the rules are expression trees; `Or` and `And` evaluate their children recursively; the terminals check whether the word occurs in the context.
- Filter: each criteria object returns a filtered list; `AndCriteria` applies one criteria to the result of the other; `OrCriteria` merges both results without duplicates (single people first, then the females not yet included).
- Null object: `findCustomer` never returns `nullptr`; unknown names get a `NullCustomer` whose `name()` returns a neutral message, so the loop needs no `if`.

**Exam relevance:** Interpreter suits small grammars (rule engines, SQL/regex parsers, expression evaluators). Filter/Criteria = composable selection rules. Null Object removes null checks and the crashes caused by forgetting them.

### 12_j2ee_patterns — Enterprise (J2EE) patterns in C++: MVC, DAO + Transfer Object, Business Delegate + Service Locator

**Question:** What does the view print before and after the controller changes the model? What does the DAO hold after update and delete? How many times is each service looked up "from the registry" when the delegate is called three times?

**Expected output:**

```text
MVC:
  view: Student Robert (roll no 10)
  view: Student John (roll no 10)
DAO + transfer object:
  all: [0 Robert] [1 John]
  dao: student 0 updated
  dao: student 1 deleted
  all: [0 Michael]
Business delegate + service locator:
  locator: looking up EJB in the registry
  processing in the EJB service
  locator: looking up JMS in the registry
  processing in the JMS service
  locator: EJB from cache
  processing in the EJB service
```

**Explanation:**

- MVC: the controller changes the model and asks the view to render it; the view only displays.
- DAO: callers use `getAll/update/remove` and never see how rows are stored. The `Student` copy returned by `getAll()` is a transfer object: changing it does nothing until it is passed back to `update()`.
- Service locator: the first lookup of each service goes to the "registry" (expensive) and is cached; the third call (EJB again) comes from the cache. The business delegate hides all of this from the presentation code.

**Exam relevance:** MVC = separation of concerns in UI code. DAO = a persistence abstraction (swap databases without touching business code). Transfer Object (DTO) = a plain data carrier between layers. Service Locator = a central, caching registry of services (an alternative to dependency injection).
