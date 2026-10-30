---
tags: [c, pointers, memory, macros, undefined-behavior, bitwise]
---

# C fundamentals

## Theory

### Data types and sizeof (x86_64 Linux, LP64)
| Type | Bytes | Notes |
| --- | --- | --- |
| `char` | 1 | signedness is implementation-defined (x86: signed, ARM: usually unsigned) |
| `short` | 2 | |
| `int` | 4 | |
| `long` | 8 | 64-bit Windows (LLP64): 4 |
| `long long` | 8 | |
| `float` | 4 | Bignum / big Decimal for number and calculation need high precision|
| `double` | 8 | contain more number behind  |
| `any pointer` | 8 | 4 on 32-bit systems |

- `sizeof` is an **operator** evaluated at compile time (except for VLAs) and yields `size_t`. `sizeof(arr)/sizeof(arr[0])` only works on real arrays, not on function parameters.
- `uint8_t`, `int32_t`... from `<stdint.h>` have fixed sizes; always use them in embedded code.

### Pointers
- `int *p[3]`: array of 3 pointers. `int (*p)[3]`: pointer to an array of 3 ints. `int (*f)(int)`: function pointer. `int *f(int)`: function returning `int*`.
- `const int *p` / `int const *p`: cannot change the **value** through p. `int *const p`: cannot change **p** itself. `const int *const p`: neither.
- `p + 1` advances by `sizeof(*p)` bytes. The difference of two pointers counts elements.
- No arithmetic on `void *` (gcc allows it as an extension); cast before dereferencing.
- Dangling pointer: points to freed memory or to a local that went out of scope. Wild pointer: never initialized. Dereferencing NULL → segfault.

### Program memory
- **text**: machine code (read-only). **rodata**: constants, string literals. **data**: initialized (non-zero) globals/statics. **bss**: uninitialized or zero globals/statics (zeroed automatically, takes no space in the file). **heap**: malloc, grows up. **stack**: locals, parameters, return addresses, grows down.
- `malloc` does not initialize; `calloc(n, size)` zeroes; `realloc` may move the block (the old pointer becomes invalid); `free(NULL)` is fine; double free → UB.
- Memory leak: losing the last pointer to a malloc'd block. Check with `valgrind --leak-check=full`.

### static, extern, volatile, const
- `static` on a local: lives for the whole program, initialized once. On a file-scope variable/function: **internal linkage** (visible only in that file).
- `extern`: declares a variable defined in another file (no storage allocated).
- `volatile`: tells the compiler the value can change outside the program flow (hardware registers, variables modified in an ISR, memory-mapped I/O), so reads/writes must not be optimized away. It does **not** make anything atomic or synchronize threads.
- `const volatile`: a read-only hardware register (the program never writes it, but it still changes).
- `register`: hint to keep in a register; you cannot take its address.

### Preprocessor and macros
- Stages: preprocess (`#include`, `#define`, `#if`) → compile → assemble → link.
- Macros are text substitution: always parenthesize parameters and the whole expression; parameters may be evaluated more than once (`MAX(i++, j)`). Wrap multi-statement macros in `do { ... } while (0)`.
- `#` stringizes a parameter, `##` pastes tokens. `__FILE__`, `__LINE__`, `__func__`.
- Include guard `#ifndef X_H / #define X_H / #endif` or `#pragma once`.
- `inline` functions are safer than macros (type checked, arguments evaluated once).

### Struct, union, bit-fields, endianness
- Padding: each member is aligned to its own alignment; the struct size is a multiple of the largest alignment. `#pragma pack(1)` / `__attribute__((packed))` removes padding.
- `union`: all members share the same memory; sizeof = largest member (plus padding).
- Bit-field `unsigned flag : 1;`: bit order and padding are implementation-defined; you cannot take its address.
- Little-endian (x86, most ARM): least significant byte at the lowest address. Big-endian: network byte order. Convert with `htonl/ntohl/htons/ntohs`.

### Common undefined behavior
Signed overflow, division by zero, dereferencing NULL or a dangling pointer, out-of-bounds access, reading an uninitialized variable, modifying a string literal, `i = i++ + ++i` (modifying a variable twice without a sequence point), shifting by ≥ the type width or left-shifting a negative number, double free.

## Quiz

### On Linux x86_64, what are `sizeof(long)` and `sizeof(void *)`? {#c-001}
- [ ] 4 and 4
- [ ] 4 and 8
- [x] 8 and 8
- [ ] 8 and 4
> 64-bit Linux uses the LP64 model: long and pointers are both 8 bytes. 64-bit Windows uses LLP64, where long is still 4.

### What is `sizeof(struct S)` on x86_64? {#c-002}
```c
struct S { char a; int b; char c; };
```
- [ ] 6
- [ ] 8
- [x] 12
- [ ] 16
> a at 0, 3 bytes padding, b at 4, c at 8, then 3 bytes of tail padding so the size is a multiple of 4 → 12. Reordering to int, char, char gives 8.

### What does `int (*p)[10];` declare? {#c-003}
- [ ] An array of 10 pointers to int
- [x] A pointer to an array of 10 ints
- [ ] A function returning a pointer to int
- [ ] A pointer to a function taking 10 ints
> The parentheses bind `*p` first: p is a pointer to `int[10]`. Without them, `int *p[10]` is an array of 10 pointers.

### Given `const int *p;`, which statement is true? {#c-004}
- [ ] p cannot be made to point to another variable
- [x] You cannot modify the value through `*p`, but you can change p
- [ ] Neither p nor `*p` can be changed
- [ ] It is equivalent to `int *const p`
> A `const` before the `*` applies to the pointed-to data. `int *const p` is the constant pointer (p cannot change).

### Where does an uninitialized global `int g;` live, and what is its value? {#c-005}
- [ ] stack, garbage
- [ ] data, 0
- [x] bss, 0
- [ ] heap, garbage
> Uninitialized globals/statics go in .bss and the loader zeroes them. .bss takes no space in the executable; only its size is recorded.

### What does `static` in front of a file-scope function do? {#c-006}
- [ ] The function can only be called once
- [ ] The function is loaded into static memory
- [x] The function is visible only within that .c file (internal linkage)
- [ ] The function becomes inline
> File-scope static limits linkage to that translation unit and avoids name clashes between files.

### When should you use `volatile`? (select all that apply) {#c-007}
- [x] A variable modified in an ISR and read in the main loop
- [x] A pointer to a memory-mapped hardware register
- [ ] A counter shared between threads, to avoid a race condition
- [ ] A constant that should be placed in ROM
> volatile stops the compiler from skipping or caching reads/writes. It does NOT make operations atomic and cannot replace a mutex/atomic between threads.

### What is the result? {#c-008}
```c
#define SQ(x) x * x
int r = SQ(2 + 3);
```
- [ ] 25
- [x] 11
- [ ] 10
- [ ] Compile error
> Text substitution gives `2 + 3 * 2 + 3` = 11. Correct form: `#define SQ(x) ((x) * (x))`.

### Why wrap a multi-statement macro in `do { ... } while (0)`? {#c-009}
- [ ] To make the macro faster
- [x] So the macro behaves like a single statement, safe after an `if` without braces and requiring a `;`
- [ ] To avoid evaluating parameters more than once
- [ ] It is required by C99
> With plain `{ ... }`, `if (c) M(); else ...` fails because the `;` ends the if. do-while(0) turns the block into one statement.

### On a little-endian machine, `uint32_t x = 0x12345678;` — which byte is at the lowest address of x? {#c-010}
- [ ] 0x12
- [x] 0x78
- [ ] 0x34
- [ ] 0x56
> Little-endian stores the least significant byte at the lowest address. Network byte order is big-endian.

### What is `sizeof(union U)` on x86_64? {#c-011}
```c
union U { char c[5]; int i; };
```
- [ ] 5
- [x] 8
- [ ] 9
- [ ] 4
> Size = largest member (5) rounded up to a multiple of the largest alignment (4, from int) → 8.

### What does `printf("%d", -1 < 1U);` print? {#c-012}
- [ ] 1
- [x] 0
- [ ] -1
- [ ] Undefined behavior
> -1 is converted to unsigned 4294967295, which is greater than 1 → 0. The classic signed/unsigned comparison trap.

### Which expression checks that unsigned x is a power of two? {#c-013}
- [ ] `x & (x + 1) == 0`
- [x] `x && !(x & (x - 1))`
- [ ] `(x | (x - 1)) == 0`
- [ ] `x % 2 == 0`
> A power of two has exactly one bit set; `x & (x-1)` clears the lowest set bit → 0. x = 0 must be excluded.

### What is the correct way to clear bit n of `reg`? {#c-014}
- [ ] `reg &= (1 << n);`
- [ ] `reg |= ~(1 << n);`
- [x] `reg &= ~(1u << n);`
- [ ] `reg ^= (1 << n);`
> Set: `|= (1u << n)`, clear: `&= ~(1u << n)`, toggle: `^= (1u << n)`, read: `(reg >> n) & 1u`.

### Which are undefined behavior? (select all that apply) {#c-015}
- [x] `int x = INT_MAX; x++;`
- [ ] `unsigned u = 0; u--;`
- [x] `char *s = "abc"; s[0] = 'A';`
- [x] `int i = 1; i = i++ + 1;`
> Signed overflow, modifying a string literal and modifying a variable twice without a sequence point are all UB. Unsigned overflow is defined as modulo 2ⁿ.

### Given `char s[] = "hello";`, what are `sizeof(s)` and `strlen(s)`? {#c-016}
- [ ] 5 and 5
- [x] 6 and 5
- [ ] 8 and 5
- [ ] 6 and 6
> The array also holds '\0' → 6 bytes; strlen does not count '\0' → 5. With `char *s`, sizeof would be 8 (a pointer).

### After `int a[5]; int *p = a;`, what are `sizeof(a)` and `sizeof(p)` (x86_64)? {#c-017}
- [ ] 5 and 8
- [x] 20 and 8
- [ ] 20 and 20
- [ ] 8 and 8
> sizeof a real array = 5 × 4. A pointer is always 8 bytes, even when it points to an array.

### `realloc(p, n)` returns a pointer different from p. What is true? {#c-018}
- [ ] You must free(p) right afterwards
- [x] The data was copied to the new block and the old p is no longer valid
- [ ] The old block can still be used alongside the new one
- [ ] realloc failed
> realloc copies and frees the old block itself. If realloc returns NULL the old block is still there — which is why `p = realloc(p, n)` without a check is a bug.

### In what order does gcc process a .c file into an executable? {#c-019}
- [ ] compile → preprocess → link → assemble
- [x] preprocess → compile → assemble → link
- [ ] preprocess → assemble → compile → link
- [ ] link → preprocess → compile → assemble
> `gcc -E` (preprocess, .i), `-S` (compile, .s), `-c` (assemble, .o), then link into an executable.

### After `int x = 5; int y = x++ * 2;`, what are x and y? {#c-020}
- [ ] x = 6, y = 12
- [x] x = 6, y = 10
- [ ] x = 5, y = 10
- [ ] x = 5, y = 12
> Postfix x++ yields the old value (5) and then increments x to 6.

### What does `printf("%d", 7 / -2);` print in C99 and later? {#c-021}
- [ ] -4
- [x] -3
- [ ] 3
- [ ] Implementation-defined
> Since C99, integer division truncates toward zero → -3. The remainder has the sign of the dividend: `7 % -2 = 1`, `-7 % 2 = -1`.
