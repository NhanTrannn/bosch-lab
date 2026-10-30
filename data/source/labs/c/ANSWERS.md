# C — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

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
