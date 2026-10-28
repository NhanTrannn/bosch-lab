# ANSWERS — Đáp án và giải thích

> Chỉ mở file này **sau khi** đã tự đoán output. Bài có kết quả **không cố định** được đánh dấu ⚠️.

## 01-c

### 01_sizeof_padding — sizeof và padding của struct

**Câu hỏi:** sizeof(struct A), sizeof(struct B), sizeof(struct C) là bao nhiêu? Offset từng member?

**Output mong đợi:**

```text
sizeof(A) = 12 | c1=0 i=4 c2=8
sizeof(B) = 8 | i=0 c1=4 c2=5
sizeof(C) = 6 | c1=0 i=1 c2=5
```

**Giải thích:**

- `struct A`: `c1` ở 0, cần 3 byte padding để `i` bắt đầu ở 4 (alignment 4), `c2` ở 8, rồi thêm 3 byte **tail padding** để tổng là bội của 4 → 12.
- `struct B`: `i` ở 0, hai `char` liền nhau ở 4 và 5, tail padding 2 byte → 8.
- `#pragma pack(1)` bỏ hết padding → 1 + 4 + 1 = 6, nhưng `i` nằm lệch (offset 1), truy cập chậm hơn và trên một số kiến trúc (ARM cũ) có thể gây lỗi bus.

**Liên hệ đề thi:** Câu hỏi kiểu "sizeof của struct {char; int; char;}" gần như chắc chắn có. Mẹo: sắp member từ lớn đến nhỏ để giảm padding. Struct rỗng trong C++ có sizeof = 1.

### 02_pointer_array — Pointer arithmetic, mảng và decay

**Câu hỏi:** sizeof(arr) trong main và sizeof(a) trong f khác nhau thế nào? *p++ in gì? pb + 1 cách pb bao nhiêu byte?

**Output mong đợi:**

```text
trong main: sizeof(arr) = 20
trong f   : sizeof(a)   = 8
*(p + 2) = 30
p[3] = 40, 3[arr] = 40
*p++ = 10, sau do *p = 20
&arr[4] - &arr[0] = 4
sizeof(pa) = 24, sizeof(pb) = 8
*pa[1] = 8, pb[1][2] = 6
(char *)(pb + 1) - (char *)pb = 12
```

**Giải thích:**

- Trong `main`, `arr` là mảng 5 int → 20 byte. Tham số `int a[10]` thực chất là `int *a` → 8 byte trên x86_64 (con số 10 bị bỏ qua).
- `a[i]` chính là `*(a + i)`, nên `3[arr]` hợp lệ và bằng `arr[3]`.
- `*p++` = `*(p++)`: lấy giá trị cũ (10) rồi mới tăng con trỏ.
- Hiệu hai con trỏ tính theo **số phần tử**, không theo byte → 4.
- `pa` là mảng 3 con trỏ → 3 × 8 = 24. `pb` chỉ là một con trỏ → 8, nhưng `pb + 1` nhảy qua cả một hàng `int[3]` = 12 byte.

**Liên hệ đề thi:** Đọc khai báo theo quy tắc "phải-trái từ tên biến": `int *p[3]` → p là mảng 3 phần tử, mỗi phần tử là `int*`. `int (*p)[3]` → p là con trỏ, trỏ tới mảng 3 int.

### 03_const_pointer — Ba kiểu const với con trỏ

**Câu hỏi:** Chương trình in gì? Lần lượt bỏ comment từng dòng đang bị comment trong main: dòng nào biên dịch được, dòng nào lỗi và lỗi gì?

**Output mong đợi:**

```text
a = 10, b = 2, *p1 = 2, *p2 = 10, *p3 = 10

Bỏ comment từng dòng:
*p1 = 10;  -> error: assignment of read-only location '*p1'
p2 = &b;   -> error: assignment of read-only variable 'p2'
*p3 = 20;  -> error: assignment of read-only location '*p3'
p3 = &b;   -> error: assignment of read-only variable 'p3'
```

**Giải thích:**

| Khai báo | Đổi con trỏ? | Đổi giá trị qua con trỏ? |
|---|---|---|
| `const int *p` (= `int const *p`) | Được | Không |
| `int *const p` | Không | Được |
| `const int *const p` | Không | Không |

`p1 = &b` hợp lệ nên `*p1` = 2. `*p2 = 10` sửa `a` thành 10, nên `*p3` cũng đọc ra 10: `const` chỉ cấm sửa **qua con trỏ đó**, không biến `a` thành hằng.

**Liên hệ đề thi:** Câu kinh điển: "Khai báo nào cho phép thay đổi con trỏ nhưng không thay đổi giá trị?" → `const int *p`. Nhớ `int const *` và `const int *` là một.

### 04_static_extern — static local, static mức file và extern

**Câu hỏi:** n và m in ra gì sau 3 lần gọi counter()? Nếu bỏ comment dòng extern int g_hidden và dùng nó thì lỗi ở bước compile hay link?

**Output mong đợi:**

```text
n = 1, m = 1
n = 2, m = 1
n = 3, m = 1
file_counter = 3
g_shared = 101, hidden = 7

Bỏ comment extern int g_hidden và dùng nó:
compile OK, nhưng LINK lỗi: undefined reference to 'g_hidden'
```

**Giải thích:**

- `static int n` nằm ở vùng data/bss, khởi tạo một lần, giữ giá trị giữa các lần gọi. `m` là biến stack, mỗi lần gọi lại bằng 0.
- `extern` chỉ là **khai báo**: compiler tin rằng biến tồn tại ở đâu đó nên compile được. Đến bước **link** mới phát hiện `g_hidden` là `static` (không xuất ra symbol toàn cục) → `undefined reference`.
- `static` trên hàm cũng có nghĩa tương tự: hàm chỉ dùng được trong file đó.

**Liên hệ đề thi:** Phân biệt **scope** (tầm nhìn) và **lifetime** (thời gian sống). `static` có 2 nghĩa: trong hàm → đổi lifetime; ngoài hàm → đổi linkage. Biến global không khởi tạo mặc định bằng 0.

### 05_bitwise — Thao tác bit

**Câu hỏi:** Viết ra 4 dòng nhị phân đầu tiên và các giá trị ở 4 dòng cuối.

**Output mong đợi:**

```text
00101100
00101101
00100100
10101100
bit 2 = 1, bit 4 = 0
so bit 1 = 3
is_pow2: 0=0 1=1 6=0 64=1
~x & 0xFF = 0xD3, x >> 2 = 11, x << 1 = 88
```

**Giải thích:**

- `0x2C` = 44 = `0010 1100`: các bit 2, 3, 5 bằng 1.
- Mỗi vòng `x &= x - 1` xoá đúng một bit 1 (thuật toán Kernighan) → số vòng = số bit 1 = 3.
- Số là luỹ thừa của 2 khi có đúng 1 bit 1 → `x & (x-1) == 0`; phải loại trừ `x = 0`.
- Dịch phải 2 = chia 4 (44/4 = 11), dịch trái 1 = nhân 2 (88).

**Liên hệ đề thi:** Hay gặp: "biểu thức nào kiểm tra luỹ thừa 2", "x & (x-1) làm gì", "x & -x" (giữ bit 1 thấp nhất), "XOR swap". Dịch trái số âm hoặc dịch ≥ số bit của kiểu là UB.

### 06_endian_union — Endianness bằng pointer và union

**Câu hỏi:** Trên PC x86_64, byte đầu tiên của 0x12345678 trong bộ nhớ là gì? Dòng "Cach 2" in 4 byte theo thứ tự nào?

**Output mong đợi:**

```text
Cach 1: byte dau = 0x78 -> little endian
Cach 2: 78 56 34 12
Cach 3: t.c = 1
sizeof(union U) = 4
```

**Giải thích:**

- x86/x86_64 và phần lớn ARM (mặc định) là **little endian**: byte `0x78` (thấp nhất) nằm ở địa chỉ thấp nhất.
- Network byte order là **big endian** → dùng `htonl/htons/ntohl/ntohs` khi gửi qua mạng.
- `sizeof(union)` = kích thước member lớn nhất (cộng padding nếu có) → 4.

**Liên hệ đề thi:** Câu "đoạn code sau in gì trên máy little endian" với `char *p = (char*)&i` là kinh điển. Nhớ: big endian giống cách viết số trên giấy.

### 07_signed_unsigned — Bẫy signed/unsigned và chia nguyên

**Câu hỏi:** Dòng A in nhánh nào? Giá trị B, C, D, E, F, G, H?

**Output mong đợi:**

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

**Giải thích:**

- A, C: `-1` chuyển sang `unsigned` thành `4294967295` (UINT_MAX) nên lớn hơn 1.
- B: unsigned không âm được, phép trừ quay vòng modulo 2^32.
- D: C99 trở đi chia nguyên **làm tròn về 0**; dấu của `%` theo **số bị chia**.
- E: chuỗi `"abc"` có thêm `'\0'` → 4 byte; `strlen` không đếm `'\0'`.
- F: unsigned tràn là định nghĩa rõ (quay về 0). **Signed** tràn mới là UB.
- H: Linux x86_64 dùng mô hình **LP64**: long và con trỏ 8 byte (Windows 64-bit là LLP64: long 4 byte).

**Liên hệ đề thi:** Rất hay ra: `if (-1 < sizeof(int))` → false vì `sizeof` trả về `size_t` (unsigned). `char` có dấu hay không là implementation-defined (x86: signed, ARM: unsigned).

### 08_macro_traps — Bẫy macro

**Câu hỏi:** Giá trị của từng dòng? Sau SQ_OK(i++) và MAX(j++, 3) thì i, j bằng bao nhiêu?

**Output mong đợi:**

```text
SQ(a + 1)     = 7
SQ_OK(a + 1)  = 16
10 / SQ(2)    = 10
10 / SQ_OK(2) = 2
DOUBLE(5) * 2 = 15
SQ_OK(i++) = 30, i = 7      <- UB: gcc thường ra 30, compiler khác có thể khác
MAX(j++, 3) = 6, j = 7
```

**Giải thích:**

- `SQ(a + 1)` → `a + 1 * a + 1` = 3 + 3 + 1 = 7.
- `10 / SQ(2)` → `10 / 2 * 2` = 10 (chia rồi nhân, trái sang phải).
- `DOUBLE(5) * 2` → `5 + 5 * 2` = 15.
- `SQ_OK(i++)` → `((i++) * (i++))`: sửa `i` hai lần không có sequence point → **undefined behavior**. Không có đáp án "đúng".
- `MAX(j++, 3)` → `((j++) > (3) ? (j++) : (3))`: `j++` chạy 2 lần → trả 6, j = 7.

**Liên hệ đề thi:** Đề hay hỏi `#define SQUARE(x) x*x` rồi `SQUARE(2+3)` → 11. Macro nhiều câu lệnh nên bọc `do { ... } while (0)`. Toán tử `#` biến tham số thành chuỗi, `##` nối token.

### 09_memory_layout — Memory layout: text, data, bss, heap, stack ⚠️ không cố định

**Câu hỏi:** Mỗi biến nằm ở vùng nhớ nào (text / rodata / data / bss / heap / stack)? Sắp xếp địa chỉ từ thấp đến cao. g_uninit và s_global bằng bao nhiêu?

**Output mong đợi:**

```text
(địa chỉ thay đổi mỗi lần chạy do ASLR, nhưng THỨ TỰ thì cố định, ví dụ)
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
(thứ tự giữa các biến CÙNG một vùng do compiler/linker quyết định)
```

**Giải thích:**

Thứ tự địa chỉ tăng dần: **text < rodata < data < bss < heap ... (khoảng trống lớn, thư viện mmap) ... stack**.

- Chuỗi hằng `"hello"` và biến `const` toàn cục nằm ở `.rodata` (chỉ đọc) → sửa `str[0]` gây **Segmentation fault**.
- `.bss` không chiếm chỗ trong file thực thi, loader tự điền 0.
- Heap mọc lên (địa chỉ tăng), stack mọc xuống (địa chỉ giảm).
- Lệnh `size` in kích thước 3 vùng `text data bss`.

**Liên hệ đề thi:** Câu hỏi: "biến static chưa khởi tạo nằm ở đâu?" → bss. "`char *s = "abc"; s[0]='x';`" → crash, còn `char s[] = "abc"` thì sửa được (mảng nằm trên stack).

### 10_switch_fallthrough — switch thiếu break

**Câu hỏi:** Với x từ 0 đến 4, mỗi dòng in gì?

**Output mong đợi:**

```text
x = 0: default four
x = 1: one two
x = 2: two
x = 3: three default four
x = 4: four 
```

**Giải thích:**

- `switch` nhảy tới nhãn khớp rồi chạy **tuần tự** cho tới `break` hoặc hết khối.
- x = 0 không khớp case nào → nhảy tới `default` (dù nó nằm giữa), rồi rơi xuống `case 4`.
- x = 4 nhảy thẳng tới `case 4`, không đi qua `default`.

**Liên hệ đề thi:** Đề hay gài `default` ở giữa hoặc đầu. Biểu thức `switch` phải là kiểu nguyên (không dùng được string/float trong C).

### 11_string_array_pointer — Chuỗi: mảng char và con trỏ tới string literal

**Câu hỏi:** sizeof và strlen của a, b, p khác nhau thế nào? sizeof("ab\0cd") và strlen của nó? sizeof('A') trong C bằng mấy? d in ra gì?

**Output mong đợi:**

```text
a: sizeof = 6, strlen = 5
b: sizeof = 10, strlen = 5
p: sizeof = 8, strlen = 5
c: sizeof = 5 (strlen(c) la UB vi khong co '\0')
"ab\0cd": sizeof = 6, strlen = 2
sizeof('A') = 4, sizeof(char) = 1
trong f: sizeof(s) = 8, strlen(s) = 5
a sau khi sua = Bosch
d = abcdefg
strcmp: 1 1 1
a == "Bosch" ? 0, strcmp = 0
```

**Giải thích:**

- `a` là mảng 6 byte (5 ký tự + `'\0'`), `b` là mảng 10 byte (phần dư là `'\0'`), `p` chỉ là con trỏ → 8 byte trên x86_64. `strlen` luôn đếm tới `'\0'` đầu tiên → 5.
- `"ab\0cd"` có 5 ký tự + `'\0'` tự thêm → sizeof 6, nhưng strlen dừng ở `\0` giữa chuỗi → 2.
- Trong **C**, `'A'` có kiểu `int` → 4. Trong **C++** `'A'` là `char` → 1.
- Tham số `char s[]` thực chất là `char *` → 8.
- `strncpy` không thêm `'\0'` khi nguồn dài hơn n → phải tự đặt `d[7] = '\0'` → còn 7 ký tự.
- `a == "Bosch"` so sánh **địa chỉ**, không so nội dung → 0. So nội dung phải dùng `strcmp` (trả 0 khi bằng).

**Liên hệ đề thi:** Phân biệt `char s[] = "..."` (sửa được, nằm trên stack) và `char *s = "..."` (literal ở .rodata, sửa → segfault). Câu hỏi "sizeof vs strlen" gần như luôn có.

### 12_precedence — Độ ưu tiên toán tử và short-circuit

**Câu hỏi:** Giá trị từng dòng? Sau dòng || và && thì i, k bằng bao nhiêu? arr[1] cuối cùng bằng mấy?

**Output mong đợi:**

```text
a & 4 == 4   -> 0
(a & 4) == 4 -> 1
1 << 2 + 1   -> 8
10 - 2 - 3   -> 5
2 * 3 % 4    -> 2
v = 10, w = 21, y = 21, arr[1] = 22
?: long nhau -> 1
(1, 2, 3)    -> 3
j = 1, i = 2
m = 0, k = 1
!5 = 0, !!5 = 1, ~5 = -6, -5 >> 1 = -3
```

**Giải thích:**

- `a & 4 == 4` = `a & (4 == 4)` = `6 & 1` = 0. Đây là bẫy kinh điển khi kiểm tra bit — luôn viết `(a & MASK) == MASK`.
- `1 << 2 + 1` = `1 << 3` = 8 vì `+` ưu tiên hơn `<<`.
- `*p++` lấy arr[0] = 10 rồi p trỏ arr[1]. `++*p` tăng arr[1] lên 21. `(*p)++` trả 21 rồi arr[1] thành 22.
- Toán tử phẩy tính lần lượt và lấy giá trị cuối → 3.
- `i++ || i++`: vế trái trả 0 (i thành 1) → phải tính vế phải, trả 1 (i thành 2) → j = 1.
- `k++ && k++`: vế trái trả 0 → **dừng**, vế phải không chạy → k = 1.
- `~5` = -6 (bù 1: ~x = -x - 1). `-5 >> 1` với số âm là implementation-defined, gcc dịch số học → -3 (làm tròn xuống, khác `-5 / 2 = -2`).

**Liên hệ đề thi:** Thứ tự ưu tiên cần nhớ (cao → thấp): `() [] -> .` › đơn ngôi `! ~ ++ -- * & sizeof` › `* / %` › `+ -` › `<< >>` › `< <= > >=` › `== !=` › `&` › `^` › `|` › `&&` › `||` › `?:` › gán › `,`.

### 13_function_pointer — Con trỏ hàm, mảng con trỏ hàm và callback (qsort)

**Câu hỏi:** Ba dòng đầu in gì? fp(1, 2) và (*fp)(1, 2) có khác nhau không? Mảng sau qsort có thứ tự nào?

**Output mong đợi:**

```text
7 + 3 = 10
7 - 3 = 4
7 * 3 = 21
fp(1, 2) = 3, (*fp)(1, 2) = 3, fp == add ? 1
sizeof(fp) = 8
5 4 3 2 1 
25 16 9 4 1 
```

**Giải thích:**

- `ops` là mảng 3 con trỏ hàm kiểu `int (*)(int, int)`; `ops[i](7, 3)` gọi hàm thứ i.
- `fp(1, 2)` và `(*fp)(1, 2)` hoàn toàn tương đương; `add` và `&add` cũng cùng giá trị.
- Comparator của qsort trả âm/0/dương. `(a < b) - (a > b)` cho thứ tự giảm dần và không bị tràn số như `b - a`.
- `apply` nhận callback `int (*fn)(int)` → bình phương từng phần tử.

**Liên hệ đề thi:** Đọc khai báo: `int (*fp)(int)` con trỏ tới hàm; `int *fp(int)` là HÀM trả về `int*`; `int (*arr[3])(void)` mảng 3 con trỏ hàm; `void (*signal(int, void (*)(int)))(int)` là prototype thật của `signal`. Dùng `typedef` cho dễ đọc.

### 14_integer_promotion — Integer promotion: char/short tự lên int

**Câu hỏi:** a + b bằng bao nhiêu, còn c? sc == uc? Dòng ~x == 0xF0 in "bang" hay "khac"? sizeof(a + b)?

**Output mong đợi:**

```text
a + b = 260
c = a + b -> 4
sizeof(a) = 1, sizeof(a + b) = 4, sizeof(+a) = 4
sc == uc ? 0  (sc -> -1, uc -> 255)
khac: ~x = 0xFFFFFFF0
hi << 1 = 256, (uint8_t)(hi << 1) = 0
un-- tu 0 = 4294967295
int sum = 60000, short sum = -5536
```

**Giải thích:**

- `a + b`: cả hai lên `int` rồi mới cộng → 260, không bị tràn. Chỉ khi gán về `unsigned char` mới cắt: 260 mod 256 = 4.
- `sc == uc`: -1 lên int vẫn là -1, 255 lên int là 255 → khác nhau, dù cùng bit pattern `0xFF`.
- `~x`: x = 0x0F lên int 0x0000000F, đảo bit → 0xFFFFFFF0 ≠ 0xF0. Phải viết `(uint8_t)~x`.
- `hi << 1` tính trên int → 256; cắt về uint8_t → 0.
- Số không dấu tràn được định nghĩa rõ (modulo 2ⁿ) → 0 - 1 = 4294967295. Số có dấu tràn là UB.
- 60000 không vừa `short` → chuyển đổi implementation-defined, gcc lấy modulo → 60000 - 65536 = -5536.

**Liên hệ đề thi:** Bẫy thật trong code driver: `uint16_t a = 0xFFFF; a * a` → nâng lên **int có dấu** rồi tràn → UB. Quy tắc: kiểu nhỏ hơn int → int; trộn có dấu/không dấu cùng hạng → không dấu.

### 15_float — Số thực: so sánh, làm tròn, chia nguyên

**Câu hỏi:** 0.1 + 0.2 == 0.3? f == 0.1? x, y, z bằng mấy? (int)-3.99? 16777217.0f in ra gì?

**Output mong đợi:**

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

**Giải thích:**

- 0.1, 0.2, 0.3 không biểu diễn chính xác trong nhị phân → tổng lệch một chút. So sánh số thực bằng `fabs(a - b) < eps`.
- `f == 0.1`: f được nâng lên double, nhưng giá trị float của 0.1 khác giá trị double của 0.1 → 0.
- `7 / 2` là chia **nguyên** → 3, gán vào double vẫn là 3.0. Chỉ cần một toán hạng là số thực → 3.5.
- Ép sang int cắt về phía 0: -3.99 → -3 (không phải -4).
- float có 24 bit mantissa → số nguyên lớn hơn 2²⁴ = 16777216 không còn chính xác.
- NaN không bằng chính nó → kiểm tra NaN bằng `x != x` hoặc `isnan`.

**Liên hệ đề thi:** Trong embedded thường tránh float (chậm nếu không có FPU) → dùng fixed-point. Không dùng `==` với số thực, không dùng float làm biến đếm vòng lặp.

### 16_scope_storage — Phạm vi biến (scope), che tên (shadowing) và static trong vòng lặp

**Câu hỏi:** Mỗi dòng x= in số mấy? Ba lần gọi f() in gì? Vòng for cuối in s, t thế nào?

**Output mong đợi:**

```text
x = 1
x = 3 (khoi trong)
x = 1 (extern)
x = 2
f: n = 1, m = 1
f: n = 2, m = 1
f: n = 3, m = 1
s = 11, t = 11
s = 12, t = 11
s = 13, t = 11
```

**Giải thích:**

- Dòng đầu chạy trước khi khai báo `int x = 2` → dùng biến toàn cục = 1.
- Trong khối, `x = 3` che x của main. `extern int x` trong khối lồng lấy lại biến toàn cục.
- `static int n` khởi tạo bằng 0 đúng một lần và giữ giá trị giữa các lần gọi; `m` là biến auto, mỗi lần gọi khởi tạo lại.
- `static int s = 10` trong thân vòng for cũng chỉ khởi tạo một lần → 11, 12, 13; `t` luôn là 11.

**Liên hệ đề thi:** 4 storage class: `auto` (mặc định, stack), `static`, `extern`, `register`. Phân biệt **scope** (nhìn thấy ở đâu) và **lifetime** (sống bao lâu): static local có scope là khối nhưng lifetime là cả chương trình.

## 02-cpp

### 01_ctor_dtor_order — Thứ tự constructor / destructor ⚠️ không cố định

**Câu hỏi:** Thứ tự các dòng ctor/dtor? m1 hay m2 được tạo trước? a bằng bao nhiêu?

**Output mong đợi:**

```text
--- tao d ---
Base ctor
Member(2) ctor
Member(1) ctor
Derived ctor, b = 10, a = <giá trị rác>
--- ra khoi scope ---
Derived dtor
Member(1) dtor
Member(2) dtor
Base dtor
--- het ---
```

**Giải thích:**

- Base luôn được tạo trước, rồi tới member theo **thứ tự khai báo trong class**: `m2` rồi `m1` — initializer list viết `m1(1), m2(2)` không thay đổi điều này.
- Tương tự `a` khai báo trước `b` nên `a(b + 1)` chạy khi `b` **chưa được khởi tạo** → `a` là giá trị rác (UB).
- Destructor đi ngược: thân `~Derived` → member (m1 rồi m2) → `~Base`.

**Liên hệ đề thi:** Đề hay cho class kế thừa nhiều tầng + member object và hỏi thứ tự in. Quy tắc: "xây từ móng lên, phá từ mái xuống".

### 02_virtual_slicing — virtual, gọi qua object/pointer/reference và object slicing

**Câu hỏi:** Mỗi dòng in "Animal", "Woof" hay "name: ..."? sizeof(Animal) và sizeof(Dog)?

**Output mong đợi:**

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

**Giải thích:**

- `Animal a = d` chỉ copy phần `Animal` → `a` thật sự là Animal (slicing), gọi `Animal::speak`.
- Qua reference/pointer: `speak` là virtual → tra vtable → `Dog::speak`.
- `name()` **không virtual** → chọn theo kiểu tĩnh của con trỏ (`Animal*`) → `name: Animal`.
- Class có hàm virtual chứa 1 con trỏ ẩn **vptr** (8 byte trên x86_64); Dog không thêm dữ liệu nên cũng 8.

**Liên hệ đề thi:** Câu hỏi "tại sao nên truyền object đa hình bằng `const&`" → tránh slicing + tránh copy. Class rỗng: sizeof = 1; thêm hàm virtual: sizeof = 8.

### 03_virtual_dtor — Destructor virtual và không virtual

**Câu hỏi:** Ở mỗi đoạn "---", destructor nào được gọi?

**Output mong đợi:**

```text
--- delete qua BaseNV* ---
~BaseNV
--- delete qua BaseV* ---
~DerivedV
~BaseV
--- object tren stack ---
~DerivedNV
~BaseNV
```

**Giải thích:**

- Không virtual: `delete p1` chỉ biết kiểu tĩnh `BaseNV*` → gọi `~BaseNV` → `buf` của DerivedNV bị leak (về mặt chuẩn là UB).
- Có virtual: destructor được tra qua vtable → `~DerivedV` rồi tự động `~BaseV`.
- Object trên stack biết chính xác kiểu của nó nên luôn huỷ đúng, không cần virtual.

**Liên hệ đề thi:** Quy tắc: class có ít nhất 1 hàm virtual (dùng làm base đa hình) thì destructor phải `virtual`. Constructor thì **không thể** virtual.

### 04_virtual_in_ctor — Gọi hàm virtual trong constructor/destructor

**Câu hỏi:** Trong Base ctor và Base dtor, hello() gọi phiên bản nào?

**Output mong đợi:**

```text
Base ctor -> Base::hello
Derived ctor -> Derived::hello
call() -> Derived::hello
Derived dtor -> Derived::hello
Base dtor -> Base::hello
```

**Giải thích:**

- Khi `Base()` chạy, phần Derived (kể cả `msg`) **chưa tồn tại**; vptr đang trỏ vtable của Base → gọi `Base::hello`. Nếu C++ cho gọi xuống Derived, `msg` chưa khởi tạo sẽ gây lỗi.
- Trong `~Base()`, phần Derived **đã bị huỷ** → cũng gọi `Base::hello`.
- `call()` là hàm thường nhưng gọi `hello()` qua `this` → dynamic dispatch bình thường → Derived.

**Liên hệ đề thi:** Câu hỏi "gọi virtual trong constructor thì sao?" → gọi bản của class đang được xây, không phải lớp con. Gọi pure virtual trong ctor → "pure virtual method called" và abort.

### 05_rule_of_five — Rule of five: copy/move ctor và assignment

**Câu hỏi:** Với các dòng đánh số 1–7, dòng nào gọi ctor, copy ctor, copy assign, move ctor, move assign? Thứ tự các dòng dtor ở cuối?

**Output mong đợi:**

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

**Giải thích:**

- (2) `Str b = a` là **khởi tạo** dù có dấu `=` → copy ctor. (4) `c = a` gán cho object đã có → copy assign.
- (5) `std::move(a)` biến `a` thành rvalue → move ctor; `a.data` = nullptr.
- (6) `Str("X")` là temporary (rvalue) → move assign, rồi temporary bị huỷ ngay cuối câu lệnh: `dtor(null)`.
- (7) C++17: trả về prvalue được **khởi tạo thẳng** vào `e` → chỉ 1 `ctor(tmp)`, không move/copy.
- Hủy theo thứ tự ngược khai báo: e(tmp), d(A), c(X), b(A), a(null).

**Liên hệ đề thi:** Rule of three: nếu tự viết 1 trong {dtor, copy ctor, copy assign} thì phải viết đủ 3. Rule of five thêm move ctor + move assign. Rule of zero: dùng RAII (string, vector, unique_ptr) để không phải viết gì.

### 06_smart_ptr — unique_ptr, shared_ptr, weak_ptr

**Câu hỏi:** use_count in ra bao nhiêu ở mỗi chỗ? Node A, Node B có được huỷ không? Fixed X/Y thì sao, theo thứ tự nào?

**Output mong đợi:**

```text
u1 rong, *u2 = 42
use_count trong block = 3
use_count sau block = 1
Node A ctor
Node B ctor
a.use_count = 2
--- sau block Node ---
x.use_count = 1
Fixed Y dtor
Fixed X dtor
--- het ---
```

**Giải thích:**

- Sau `move`, `u1` rỗng (nullptr). Copy `unique_ptr` là lỗi biên dịch.
- `s1, s2, s3` cùng sở hữu → 3; ra khỏi block còn 1.
- A giữ B, B giữ A: khi biến `a`, `b` bị huỷ, mỗi Node vẫn còn count = 1 → **không bao giờ in "Node ... dtor"** → memory leak.
- Với `weak_ptr`, count của x là 1; ra khỏi block, `y` (khai báo sau) huỷ trước → `Fixed Y dtor`, rồi `Fixed X dtor`.

**Liên hệ đề thi:** Hay hỏi: `make_shared` cấp phát 1 lần cho cả object + control block. `shared_ptr` thread-safe ở bộ đếm, không phải ở object. Dùng `weak_ptr::lock()` để lấy `shared_ptr` tạm thời.

### 07_lambda_capture — Lambda capture theo giá trị và tham chiếu

**Câu hỏi:** byVal, byRef in gì sau khi x đổi thành 20? inc() lần 3 trả về gì, counter ngoài bằng bao nhiêu? all() và dòng cuối?

**Output mong đợi:**

```text
byVal = 10, byRef = 20
inc() = 3, counter = 0
all() = 20, counter = 5
0 1 4 
```

**Giải thích:**

- `byVal` chụp `x = 10` lúc tạo; `byRef` đọc biến thật → 20.
- `inc` có bản copy riêng của `counter` (được giữ giữa các lần gọi vì lambda là object) → 1, 2, 3. Biến `counter` bên ngoài vẫn 0.
- `all` chụp `x = 20, counter = 0` lúc tạo → 20, dù sau đó `allRef` tăng counter thật lên 5.
- Mỗi lambda trong vòng for chụp giá trị `i` riêng → 0 1 4.

**Liên hệ đề thi:** Lambda thực chất là một class ẩn có `operator()` `const`; biến capture theo giá trị là member của nó. Capture `this` trong member function để truy cập member.

### 08_stl_traps — Bẫy STL: map[], erase trong vòng lặp, iterator invalidation

**Câu hỏi:** size của map sau khi đọc m["b"]? Vector v còn gì? capacity trước/sau và p còn trỏ tới w[0] không?

**Output mong đợi:**

```text
size truoc = 1
m["b"] == 0
size sau [] = 2
count("c") = 0, size = 2
1 3 5 | size = 3
capacity = 2
capacity = 4
p == &w[0] ? no
```

**Giải thích:**

- Đọc `m["b"]` chèn `{"b", 0}` → size 2. Muốn kiểm tra tồn tại thì dùng `count()` hoặc `find()` (không chèn).
- Vòng xoá đúng: lấy iterator `erase` trả về. Viết `v.erase(it); ++it;` là dùng iterator đã invalid → UB.
- libstdc++ nhân đôi capacity khi đầy: 2 → 4. Vùng nhớ mới khác vùng cũ nên `p` trở thành **dangling pointer**.

**Liên hệ đề thi:** `reserve` đổi capacity, `resize` đổi size. `map` sắp xếp theo key (cây đỏ-đen, O(log n)); `unordered_map` là hash table (O(1) trung bình, không có thứ tự).

### 09_name_hiding — Name hiding và using Base::f

**Câu hỏi:** Các lời gọi trên d và x in gì? Nếu bỏ comment d.f(1) thì có biên dịch được không?

**Output mong đợi:**

```text
Derived::f(const char*)
Base::f(int)
Base::g(int)
Fixed::f(const char*)
Base::f(int)
Base::f(double)
Base::f(int)

Bỏ comment d.f(1):
error: invalid conversion from 'int' to 'const char*'
```

**Giải thích:**

- Tra tên dừng ở scope **đầu tiên** tìm thấy `f` (Derived) rồi mới xét overload → `Base::f(int)` không được xem xét → lỗi khi gọi `d.f(1)`.
- `g` không bị che vì Derived không khai báo `g`.
- `x.f('A')`: `char` → `int` là **promotion**, ưu tiên hơn conversion sang `double` → `Base::f(int)`.

**Liên hệ đề thi:** Phân biệt: **overload** (cùng scope, khác tham số, compile-time), **override** (virtual, cùng chữ ký, runtime), **hiding** (lớp con trùng tên che lớp cha).

### 10_virtual_default_arg — Tham số mặc định của hàm virtual và gọi virtual qua hàm thường

**Câu hỏi:** Mỗi dòng in "Base::f" hay "Derived::f", và x bằng 1 hay 2?

**Output mong đợi:**

```text
1: Derived::f x=1
2: Derived::f x=1
3: Derived::f x=2
4: Base::f x=1
5: Derived::f x=1
6: Base::f x=1
7: Derived::f x=5
```

**Giải thích:**

- 1, 2: gọi qua `Base*`/`Base&` → hàm của Derived (dynamic binding), nhưng tham số mặc định lấy theo kiểu tĩnh `Base` → x = 1.
- 3: gọi qua `Derived` → kiểu tĩnh là Derived → x = 2.
- 4: `pb->Base::f()` gọi có tên lớp → tắt cơ chế virtual, gọi thẳng Base::f.
- 5: trong `run()`, `f()` là `this->f()` với `this` kiểu `Base*` → Derived::f, x = 1.
- 6: `b` là object Base thật (bị slicing) → Base::f.
- 7: truyền tham số rõ ràng → không dính bẫy.

**Liên hệ đề thi:** "Virtual function dùng default argument của lớp nào?" — của kiểu tĩnh (static type). Effective C++: không bao giờ định nghĩa lại tham số mặc định của hàm virtual được kế thừa.

### 11_exception_raii — Exception, stack unwinding và RAII

**Câu hỏi:** Thứ tự các dòng +/− ? catch nào bắt được? Bắt bằng giá trị in what() ra gì? ~Bad có chạy không?

**Output mong đợi:**

```text
+main
+L1
+L2
-L2
-L1
-main
main bat: loi o level2
bat bang gia tri: std::exception
int 42
+member
-member
constructor cua Bad nem loi
```

**Giải thích:**

- Exception đi ngược lên stack: huỷ L2, catch `logic_error` ở level1 không khớp (`runtime_error` không phải con của `logic_error`) → huỷ L1 → huỷ `main` guard **trước** khi vào khối catch.
- Dòng "level1 ket thuc binh thuong" không in vì exception không bị bắt ở level1.
- Bắt `std::exception` bằng giá trị → copy phần Base (slicing) → `what()` của std::exception trả `"std::exception"` (libstdc++).
- catch được thử theo thứ tự viết, không có chuyển đổi kiểu (int không khớp double).
- Constructor của Bad ném → object chưa "sống" nên `~Bad` không chạy, nhưng member đã tạo xong (`member`) được huỷ.

**Liên hệ đề thi:** RAII = Resource Acquisition Is Initialization: lấy tài nguyên trong constructor, trả trong destructor → không rò rỉ kể cả khi có exception. Không ném exception ra khỏi destructor (mặc định destructor là `noexcept`).

### 12_diamond — Đa kế thừa, diamond và virtual inheritance

**Câu hỏi:** D d; in thứ tự constructor nào? A được tạo mấy lần? Với VD thì sao? Thứ tự E (khai báo C trước B)?

**Output mong đợi:**

```text
D : A B A C D
d.B::x = 1, d.C::x = 2
VD: A VB VC VD
vd.VB::x = 7, vd.VC::x = 7
E : A C A B E
sizeof: A=4 D=8 VD=24
```

**Giải thích:**

- D kế thừa B rồi C, mỗi đứa có một A riêng → A được tạo 2 lần, D chứa 2 bản `x`.
- Kế thừa `virtual A` → VB và VC dùng chung một A, constructor của A được gọi **trước tiên** bởi lớp cuối cùng VD.
- E viết initializer `B(), C()` nhưng khai báo `: C, B` → thứ tự thật là C rồi B (gcc cảnh báo -Wreorder).
- sizeof(VD) = 24: phần VB và phần VC mỗi phần có một vptr (8 + 8) — vtable chứa offset tới phần A dùng chung — cộng A (4) và padding.

**Liên hệ đề thi:** Thứ tự khởi tạo: virtual base → base theo thứ tự khai báo → member theo thứ tự khai báo → thân constructor. Huỷ theo thứ tự ngược lại.

### 13_static_member — static member, static local và singleton

**Câu hỏi:** Các dòng "tao/copy/huy" theo thứ tự nào? make() có gọi copy không? Logger() in mấy lần? sizeof(Counter)?

**Output mong đợi:**

```text
alive=0 created=0
tao #1
copy #1 -> #2
copy #1 -> #3
alive=3 created=3
huy #3
tao #4
alive=3 created=4
huy #4
huy #2
huy #1
alive=0 created=4
Logger()
n = 2, cung 1 object? 1
sizeof(Counter) = 4
```

**Giải thích:**

- `Counter b = a` → copy #2. `by_value(a)` → copy vào tham số #3; tham số bị huỷ khi hàm kết thúc (huy #3).
- `make()` trả về prvalue → C++17 bảo đảm copy elision, chỉ có "tao #4", không copy.
- Cuối khối: huỷ ngược thứ tự tạo: c(#4), b(#2), a(#1).
- `static Logger inst` chỉ khởi tạo ở lần gọi đầu → "Logger()" in một lần; mọi lần gọi trả cùng object.
- sizeof chỉ tính member không static → 4.

**Liên hệ đề thi:** static member function không có `this` nên không truy cập được member thường và không thể là virtual/const. Meyers singleton (static local) thread-safe từ C++11.

### 14_move_noexcept — std::move, const và noexcept khi vector cấp phát lại

**Câu hỏi:** Khi vector tăng capacity, phần tử cũ được copy hay move với A và B? std::move(ca) gọi gì? s sau khi bị move còn gì?

**Output mong đợi:**

```text
A, tang capacity: copy copy 
B, tang capacity: move move 
push_back(lvalue): copy | push_back(std::move): move | emplace_back(): 
A x = std::move(ca): copy 
s = [], t = [hello]
```

**Giải thích:**

- Phần tử thứ 3 vượt capacity 2 → vector cấp phát vùng mới và chuyển 2 phần tử cũ sang. A có move constructor **không** noexcept → vector dùng `std::move_if_noexcept` và chọn copy. B có `noexcept` → move.
- Phần tử mới tạo bằng `emplace_back()` được dựng tại chỗ → không in gì.
- `std::move(ca)` cho kiểu `const A&&` → không khớp `A(A&&)` → chọn `A(const A&)` → copy.
- String sau khi bị move ở trạng thái "hợp lệ nhưng không xác định"; libstdc++ để rỗng, nhưng không được dựa vào điều đó.

**Liên hệ đề thi:** Luôn đánh dấu move constructor/move assignment là `noexcept`. `std::move` = `static_cast<T&&>`. Không viết `return std::move(local)` vì chặn NRVO.

### 15_sizeof_class — sizeof của class: class rỗng, vptr, padding, static

**Câu hỏi:** sizeof của từng struct trên x86_64? Hai object Empty có cùng địa chỉ không?

**Output mong đợi:**

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

**Giải thích:**

- Class rỗng = 1 byte để hai object khác nhau có địa chỉ khác nhau.
- Hàm thành viên và static không chiếm chỗ trong object.
- Có virtual → thêm vptr 8 byte. `IntVirtual` = vptr 8 + int 4 + padding 4 = 16. Thêm bao nhiêu hàm virtual nữa thì sizeof vẫn vậy (chỉ vtable dài ra).
- Empty Base Optimization: lớp cơ sở rỗng không tốn byte → DerivedEmpty = 4. Nhưng hai **member** rỗng thì mỗi cái 1 byte → 2.
- `char, double, char` → 1 + 7 pad + 8 + 1 + 7 pad = 24; sắp lại `double, char, char` → 16.

**Liên hệ đề thi:** "sizeof class rỗng trong C++" = 1 (trong C, struct rỗng là extension của gcc với sizeof 0). vtable: một bảng mỗi class, vptr: một con trỏ mỗi object.

### 16_templates — Template: suy luận kiểu, specialization, overload với hàm thường

**Câu hỏi:** Mỗi lời gọi maxv chọn bản nào: hàm thường, template, hay bản specialization? Box<int>::count và Box<double>::count bằng mấy? Fact<5>::v?

**Output mong đợi:**

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

**Giải thích:**

- `maxv(1, 2)`: hàm thường và template khớp như nhau → ưu tiên hàm thường.
- `maxv(1.5, 2.5)`: chỉ template khớp chính xác (T = double).
- `maxv<int>(3, 4)`: chỉ định template tường minh → bắt buộc dùng template.
- `maxv("abc", "abd")`: T suy ra `const char*` → dùng bản specialization (so nội dung bằng strcmp, không so địa chỉ).
- `maxv(1, 2.5)`: template không suy ra được T (int hay double?) → bị loại; hàm thường nhận được nhờ chuyển 2.5 → 2.
- Box<int> và Box<double> là hai class khác nhau → static riêng.
- Fact tính lúc biên dịch: 5! = 120.

**Liên hệ đề thi:** Template phải đặt định nghĩa trong header (vì được sinh code lúc dùng). Specialization toàn phần dùng `template <>`; class template có partial specialization, function template thì không.

### 17_types_conversion — Biến, kiểu cơ bản, chia nguyên và chuyển kiểu

**Câu hỏi:** 7/2, 7%2, -7/2, -7%2? avg = (1+2+4)/3 bằng mấy? 'A'+1 in ra gì? unsigned 0 trừ 1? 10/3*3 và 10*3/3?

**Output mong đợi:**

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

**Giải thích:**

- Chia nguyên cắt về 0 (C++11 trở đi): -7/2 = -3, và dấu của % theo số bị chia: -7 % 2 = -1.
- `static_cast<double>(a) / b`: ép trước → chia thực 3.5. `static_cast<double>(a / b)`: chia nguyên trước (3) rồi mới ép → 3.
- `(1 + 2 + 4) / 3` toàn int → 7 / 3 = 2, gán vào double vẫn là 2.
- `10 / 3 * 3`: tính trái sang phải, (10 / 3) = 3, × 3 = 9. `10 * 3 / 3` = 30 / 3 = 10.
- Gán double vào int bằng `=` cắt phần thập phân (về phía 0); `int bad{3.99}` báo lỗi narrowing.
- `c + 1`: char lên int → 66 (in số). Muốn in ký tự phải ép lại `char`.
- unsigned tràn được định nghĩa: quay vòng modulo 2³² → 4294967295.
- Mọi giá trị khác 0 chuyển sang bool là `true` (in ra 1, hoặc `true` khi bật boolalpha).
- sizeof(long) = 8 trên Linux 64-bit (LP64); Windows 64-bit là 4 (LLP64).

**Liên hệ đề thi:** Câu kinh điển "tính trung bình bị sai" do chia nguyên. `const` có thể là hằng lúc chạy; `constexpr` bắt buộc tính được lúc biên dịch nên dùng được làm kích thước mảng.

### 18_operators — Toán tử: ++/--, gán kép, so sánh, logic và short-circuit

**Câu hỏi:** i, j, k sau i++ và ++i? a sau chuỗi gán kép? cnt sau true || inc()? 0.1 + 0.2 == 0.3? 1 < 2 < 3 và 3 > 2 > 1? 5 / 2 * 2.0?

**Output mong đợi:**

```text
sau j = i++: i = 6, j = 5
sau k = ++i: i = 7, k = 7
a = 4
m *= 2 + 1 -> 9
(n != 0) && (10 / n > 1) = 0 (không chia cho 0)
t1 t2 t3 = 101, cnt = 1
true || false && false = 1
!0 + 1 = 2, !(0 + 1) = 0
0.1 + 0.2 == 0.3 ? 0, |diff| < 1e-9 ? 1
1 < 2 < 3 = 1, 3 > 2 > 1 = 0
5 / 2 * 2.0 = 4, 5 / 2.0 * 2 = 5
2 + 3 * 4 = 14, (2 + 3) * 4 = 20, 10 - 4 - 3 = 3
```

**Giải thích:**

- `j = i++`: j nhận giá trị cũ 5, i thành 6. `k = ++i`: i tăng lên 7 rồi k nhận 7.
- Chuỗi gán kép: 10 + 10 = 20 → 17 → 34 → 34 / 3 = 11 → 11 % 7 = 4.
- `m *= 2 + 1` là `m = m * (2 + 1)` = 9 (gán kép có độ ưu tiên thấp nhất), không phải `m * 2 + 1`.
- `(n != 0) && ...`: vế trái false nên vế phải không chạy, không chia cho 0.
- `true || inc()` và `false && inc()`: không gọi inc(). Chỉ `false || inc()` gọi → cnt = 1.
- `&&` ưu tiên hơn `||`: `true || (false && false)` = true.
- `!0 + 1`: `!` là toán tử một ngôi, ưu tiên cao hơn `+` → (!0) + 1 = 2.
- 0.1, 0.2, 0.3 không biểu diễn chính xác được trong nhị phân → so sánh bằng epsilon.
- `3 > 2 > 1` = `(3 > 2) > 1` = `1 > 1` = false.
- `5 / 2 * 2.0`: chia nguyên trước (2) rồi × 2.0 = 4.

**Liên hệ đề thi:** Trong một biểu thức không được sửa cùng một biến hai lần mà không có điểm sắp xếp thứ tự, ví dụ `i = i++ + ++i` là UB. Short-circuit thường dùng để kiểm tra con trỏ: `if (p && p->x > 0)`.

### 19_control_flow — Điều khiển luồng: range-for copy/ref, do-while, continue/break, dangling else, ?:

**Câu hỏi:** Vector sau vòng for (auto x : v) và for (auto &x : v)? do-while chạy mấy lần khi điều kiện sai ngay? break trong vòng lặp lồng thoát mấy vòng? else thuộc if nào? (true ? 1 : 2.5) / 2?

**Output mong đợi:**

```text
sau for (auto x : v):  1 2 3 
sau for (auto &x : v): 10 20 30 
do-while chay 1 lan
continue/break: 0 2 4 6 
break trong vong long: 00 10 20 
else thuoc if (y)
score 75: pass
(true ? 1 : 2.5) / 2 = 0.5
ngay thuong
for 2 bien: sum = 25
```

**Giải thích:**

- `auto x` là bản copy, sửa x không đổi v. `auto &x` là tham chiếu tới phần tử, sửa thẳng vào v.
- do-while kiểm tra điều kiện ở cuối nên thân chạy ít nhất một lần. while kiểm tra trước nên không chạy lần nào.
- k lẻ thì `continue` bỏ qua; k = 8 > 6 thì `break` → in 0 2 4 6.
- `break` chỉ thoát vòng trong (theo c), vòng ngoài (theo r) vẫn chạy đủ 3 lần → 00 10 20.
- Thụt lề đánh lừa: `else` gắn với `if (y)` gần nhất. x = 1, y = 0 → vào else.
- Hai nhánh của `?:` là int và double → kiểu chung là double → 1.0 / 2 = 0.5.
- `for (a = 1, b = 10; a < b; ++a, --b)`: cộng 9 + 7 + 5 + 3 + 1 = 25.

**Liên hệ đề thi:** Đặt `;` ngay sau `for (...)` hoặc `if (...)` làm thân rỗng. `switch` chỉ nhận kiểu số nguyên/enum/char, không nhận string hay double; các case gộp nhiều giá trị bằng cách viết liền không có break.

### 20_strings — C-style string và std::string

**Câu hỏi:** sizeof(s) và strlen(s) của char s[] = "Hello"? s1 == s2 (mảng char) so sánh gì? substr, find, npos? "Zebra" < "apple"? stoi("42abc")? cin >> đọc "Frank Smith" được gì?

**Output mong đợi:**

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

**Giải thích:**

- `sizeof` tính cả `'\0'` (6); `strlen` đếm tới trước `'\0'` (5).
- `s1 == s2` với mảng char: hai mảng decay thành con trỏ → so sánh địa chỉ → false. Phải dùng `strcmp`, nó trả về 0 khi bằng nhau.
- `strcpy`/`strcat` không kiểm tra kích thước: buf phải đủ chỗ (11 ký tự + `'\0'`).
- `std::string` so sánh nội dung theo thứ tự từ điển, dựa trên mã ASCII. 'Z' = 90 < 'a' = 97 nên "Zebra" < "apple".
- `substr(pos, len)`; `find` trả về vị trí đầu tiên hoặc `string::npos` nếu không thấy.
- `size()` và `length()` giống hệt nhau. `insert(0, ">>")` → ">>ABCD", rồi `erase(2, 1)` xoá 'A'.
- `at()` kiểm tra biên và ném `out_of_range`; `operator[]` không kiểm tra (vượt biên là UB).
- `stoi` đọc phần số ở đầu chuỗi và dừng ở ký tự không phải số; chuỗi không bắt đầu bằng số thì ném `invalid_argument`.
- `>>` bỏ khoảng trắng đầu và dừng ở khoảng trắng → "Frank". `getline` đọc phần còn lại của dòng, kể cả dấu cách → " Smith".

**Liên hệ đề thi:** Bẫy `cin >> n;` rồi `getline(cin, s);`: getline đọc ngay ký tự '\n' còn sót lại và trả về chuỗi rỗng. Cách sửa là gọi `cin.ignore()` hoặc `cin >> ws` trước getline.

### 21_functions — Hàm: tham trị/tham chiếu/con trỏ, tham số mặc định, overload, static local, đệ quy

**Câu hỏi:** x sau by_val/by_ref/by_ptr? print(5.0f), print('a'), print("hi") gọi bản nào? counter() ba lần? fib(5) gọi bao nhiêu lần?

**Output mong đợi:**

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
fib(5) = 5, so lan goi = 15
factorial(10) = 3628800
```

**Giải thích:**

- by_val sửa bản copy; by_ref và by_ptr sửa biến gốc. swap_val không đổi được a, b.
- `print(5.0f)`: không có bản float, float → double là promotion → bản double.
- `print('a')`: có bản char khớp chính xác.
- **`print("hi")` gọi bản bool!** `const char*` → `bool` là chuyển đổi chuẩn, được ưu tiên hơn `const char*` → `std::string` (chuyển đổi do người dùng định nghĩa, qua constructor của string).
- Tham số mặc định được điền từ phải sang trái; không bỏ qua tham số giữa được.
- `static int c` chỉ khởi tạo ở lần gọi đầu và giữ giá trị giữa các lần gọi → 1 2 3.
- fib đệ quy ngây thơ: số lần gọi C(n) = C(n-1) + C(n-2) + 1 → C(5) = 15, tăng theo hàm mũ.

**Liên hệ đề thi:** Overload chỉ dựa vào danh sách tham số (số lượng, kiểu, const của tham số tham chiếu/con trỏ), không dựa vào kiểu trả về. `print(5L)` báo lỗi ambiguous vì long → int và long → double cùng hạng. Mỗi lần gọi hàm tạo một stack frame mới; đệ quy quá sâu gây stack overflow (segfault).

### 22_pointers_refs — Con trỏ và tham chiếu

**Câu hỏi:** a, b sau r = b và *p = 99? *(q+2), q[3], *q+1, &arr[3]-q? reset_val có làm p trong main thành nullptr không, còn reset_ref? sizeof(p) và sizeof(r)?

**Output mong đợi:**

```text
sau r = b: a = 20, b = 20, &r == &a ? 1
sau r = 30: a = 30, b = 20
sau p = &b; *p = 99: a = 30, b = 99
*(q+2) = 30, q[3] = 40, *q+1 = 11, &arr[3]-q = 3
sau ++q: *q = 20, q[-1] = 10
reset_val: pa la nullptr ? 0
reset_ref: pa la nullptr ? 1
sizeof(p) = 8, sizeof(r) = 4, sizeof(arr) = 16
*pc = 99, *cp = 1
*one = 5, many = 777
ps->size() = 3, (*pv)[1] = Larry, pv->at(0) = Frank
**pp = 5 -> b = 5
```

**Giải thích:**

- `r = b` không làm r tham chiếu tới b mà gán giá trị của b (20) cho a. r vẫn là a.
- Con trỏ trỏ lại được: `p = &b` rồi `*p = 99` sửa b.
- `*q + 1`: `*` ưu tiên hơn `+` → 10 + 1 = 11. `&arr[3] - q` là số phần tử giữa hai con trỏ (3), không phải số byte.
- `q[-1]` hợp lệ vì q đang trỏ vào giữa mảng.
- reset_val nhận bản copy của con trỏ → con trỏ của main không đổi. Muốn đổi phải truyền `int*&` (hoặc `int**`).
- `sizeof(r)` là sizeof của biến được tham chiếu (int = 4). `sizeof(p)` là kích thước con trỏ (8 trên 64-bit).
- `const int *pc` cho phép đổi pc nhưng không sửa *pc; `int *const cp` thì ngược lại. pc đang trỏ b = 99, còn `*cp = 1` sửa a.
- `delete one` sau khi `one = nullptr` không làm gì cả, an toàn.
- `**pp`: pp trỏ tới p, p đang trỏ b.

**Liên hệ đề thi:** Reference: bắt buộc khởi tạo, không null, không trỏ lại được. Con trỏ: có thể null, trỏ lại được, có pointer arithmetic. Lỗi hay gặp: dùng con trỏ sau khi delete (dangling), delete hai lần, quên delete (leak), dùng `delete` cho vùng cấp bằng `new[]`.

### 23_class_basics — Class: constructor, delegating constructor, copy constructor, const, static, shallow/deep copy

**Câu hỏi:** Player a; in ra những dòng nào và theo thứ tự nào? display(c) gọi copy constructor không? Player::count ở các thời điểm? Thứ tự destructor khi ra khỏi khối? Shallow copy hai con trỏ có cùng địa chỉ không?

**Output mong đợi:**

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
count = 4, ra khoi khoi:
  ~Clone
  ~Hero
  ~Frank
  ~None
count = 0
  three-args Villain
  one-arg
v.get_name() = Villain
Shallow: cung dia chi ? 1, *s1.data = 42
Deep: cung dia chi ? 0, *d1.data = 1
het main:
  ~Villain
```

**Giải thích:**

- Delegating constructor: constructor 3 tham số chạy hết (kể cả thân) rồi mới tới thân của constructor ủy quyền → "three-args" in trước "no-args".
- `display(Player p)` truyền theo giá trị → gọi copy constructor; bản copy p bị huỷ khi hàm kết thúc (`~Hero` đầu tiên).
- `Player d = c;` là khởi tạo, không phải gán → gọi copy constructor.
- Huỷ ngược thứ tự tạo: d (đã đổi tên Clone), c, b, a.
- static count dùng chung cho mọi object: tăng trong constructor, giảm trong destructor.
- Object const chỉ gọi được hàm member const.
- Shallow: copy mặc định copy giá trị con trỏ → s1, s2 cùng trỏ một int; sửa qua s2 thấy ở s1. Nếu cả hai cùng delete thì bị double free.
- Deep: copy constructor tự viết cấp phát vùng nhớ mới.

**Liên hệ đề thi:** Compiler chỉ sinh default constructor khi class không có constructor nào. Copy constructor được gọi khi: khởi tạo từ object khác, truyền tham trị, trả về theo giá trị (có thể bị bỏ qua nhờ copy elision). Class có con trỏ sở hữu tài nguyên → cần rule of three/five.

### 24_operator_overloading — Operator overloading

**Câu hỏi:** c = a++ thì c và a là gì? Có bao nhiêu dòng [pre++] khi gọi a++? 2 * a gọi được không, a * 2 thì sao? (a += b) += b cho kết quả gì?

**Output mong đợi:**

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

**Giải thích:**

- Postfix `operator++(int)`: lưu bản cũ, gọi prefix (in `[pre++]`), rồi trả về bản cũ → c = (1,2), a = (2,3).
- Prefix trả về `*this` theo tham chiếu, không tạo bản copy.
- `2 * a`: vế trái là int nên phải là hàm non-member (ở đây là friend). `a * 2` lỗi vì chưa định nghĩa.
- `operator[]` trả về `int&` nên gán được `a[1] = 9`.
- `+=` trả về `Vec&` → `(e += b) += b` cộng b hai lần vào cùng e: (1,1) + (3,4) + (3,4) = (7,9).
- Constructor không `explicit` cho phép chuyển ngầm: `Vec f = 5` và `f + 1` (1 → Vec(1,0)).

**Liên hệ đề thi:** Không overload được `.` `.*` `::` `?:` `sizeof`. Toán tử bắt buộc là member: `=`, `[]`, `()`, `->`. `<<` và `>>` với stream phải là non-member. Overload không đổi được độ ưu tiên hay số toán hạng.

### 25_inheritance — Kế thừa: constructor lớp cha, redefine và virtual

**Câu hỏi:** Tạo Trust in ra các constructor theo thứ tự nào? Qua Account& gọi withdraw (không virtual) và print (virtual) chạy bản nào? Vòng lặp qua vector<Account*> in gì? Thứ tự destructor khi delete?

**Output mong đợi:**

```text
Tao Trust t:
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
Tao vector<Account*>:
  Account(A)
  Account(S)
  Savings(S)
In qua Account*:
  [Account A: 10]
  [Savings S: 20, 3%]
delete:
  ~Account(A)
  ~Savings(S)
  ~Account(S)
sh.area() = 9
het main:
  ~Trust(Tom)
  ~Savings(Tom)
  ~Account(Tom)
```

**Giải thích:**

- Constructor đi từ lớp gốc xuống: Account → Savings → Trust. Destructor ngược lại.
- `withdraw` không virtual → static binding: qua `Account&` gọi `Account::withdraw`, qua object Trust gọi `Savings::withdraw` (bản gần nhất trong chuỗi kế thừa).
- `print` virtual → dynamic binding: qua `Account&` vẫn gọi `Trust::print`. Số dư 800 vì đã rút 2 lần 100.
- `~Account` là virtual nên `delete` qua `Account*` gọi đủ `~Savings` rồi `~Account`.
- Lớp có hàm thuần ảo (`= 0`) là abstract, không tạo object được nhưng dùng làm tham chiếu/con trỏ được.

**Liên hệ đề thi:** `public` inheritance giữ nguyên quyền truy cập, `protected` biến public thành protected, `private` biến tất cả thành private; member private của lớp cha không truy cập được từ lớp con. Lớp con không kế thừa constructor, destructor, operator= (trừ khi dùng `using Base::Base`). `override` giúp compiler báo lỗi nếu chữ ký không khớp.

### 26_exceptions — Exception: thứ tự catch, catch(...), rethrow, exception tự định nghĩa

**Câu hỏi:** Mỗi lần gọi test(k) được catch bởi khối nào? Dòng "sau test(k)" in ra với những k nào? Bắt theo giá trị (catch(std::exception e)) thì what() trả về gì? stoi("abc") và v.at(10) ném kiểu gì?

**Output mong đợi:**

```text
  catch int 42
  catch string: loi dang string
  catch InsufficientFunds: Insufficient funds, need = 50
  catch exception: tham so sai
  catch (...)
  k = 5: khong throw
  sau test(5)
  rethrow(): bat duoc "tu rethrow", nem lai
  main bat lai: tu rethrow
  catch theo gia tri: what() = std::exception
  stoi("abc") -> invalid_argument
  v.at(10) -> out_of_range
```

**Giải thích:**

- Ném được mọi kiểu: int, string, double, class. Khối catch được thử theo thứ tự và không có chuyển đổi kiểu số (3.14 không vào `catch (int)`) → rơi xuống `catch (...)`.
- `invalid_argument` kế thừa `logic_error` → `exception` → vào `catch (const exception&)`.
- InsufficientFunds cũng là `exception`, nhưng `catch (const InsufficientFunds&)` đứng trước nên được chọn.
- Sau `throw`, phần còn lại của khối try bị bỏ qua → "sau test(k)" chỉ in khi k = 5.
- `throw;` ném lại đúng object gốc (giữ kiểu động), lên khối try bên ngoài.
- `catch (exception e)` copy phần base → slicing → `what()` của std::exception ("std::exception").

**Liên hệ đề thi:** Ném theo giá trị, bắt theo const reference. Không ném exception từ destructor (trong lúc stack unwinding sẽ gọi `std::terminate`). Exception không được bắt → `std::terminate` → abort.

### 27_streams — I/O stream: iomanip, manipulator dính/không dính, stringstream

**Câu hỏi:** 1234.5678 in mặc định ra gì? setprecision(2) có và không có fixed? setw áp dụng cho mấy lần in? hex có giữ cho lần in sau không? Đọc "12 abc" vào hai biến int thì sao?

**Output mong đợi:**

```text
mac dinh:            1234.57, 1234567.89 -> 1.23457e+06
setprecision(2):     1.2e+03
fixed setprecision(2): 1234.57
van con fixed:       3.14
[    42][42]
[42    ][ab    ]
[****42][**7]
ff 10 0xff 255
10 false 2.00000 2
doc: Frank | 100 | 1234.5
"12 abc 34": x = 12, y = 0, z = 0, fail = 1
sau clear(): skip = abc, z = 34
ostringstream: [Total:    42!]
123abc khong phai so nguyen (doc duoc 123)
```

**Giải thích:**

- Mặc định in 6 chữ số có nghĩa: 1234.5678 → 1234.57; số lớn chuyển sang dạng khoa học 1.23457e+06.
- `setprecision(2)` không có fixed: 2 chữ số có nghĩa → 1.2e+03. Thêm `fixed`: 2 số sau dấu phẩy → 1234.57.
- `fixed` và precision giữ nguyên cho các lần in sau (3.14).
- `setw` chỉ áp dụng cho lần in kế tiếp; mặc định canh phải, `left` canh trái và được giữ lại. `setfill` cũng được giữ lại.
- `hex` được giữ lại (16 → 10) tới khi gặp `dec`; `showbase` thêm tiền tố 0x. `oct` 8 → 10. `showpoint` in đủ 6 chữ số có nghĩa: 2.00000.
- Đọc "abc" vào int thất bại: y bị gán 0 (C++11) và stream vào trạng thái fail, z không được đọc. Sau `clear()` mới đọc tiếp được.
- "123abc": đọc được 123 nhưng chưa tới cuối chuỗi (eof false) → không phải số nguyên hợp lệ.

**Liên hệ đề thi:** Phân cấp: `ios` → `istream`/`ostream` → `iostream`; file dùng `ifstream`/`ofstream`/`fstream`; chuỗi dùng `istringstream`/`ostringstream`/`stringstream`. Mở file luôn kiểm tra `is_open()`. Đọc từng dòng: `while (getline(file, line))`, đừng dùng `while (!file.eof())` vì vòng cuối sẽ bị lặp thừa.

### 28_stl_algorithms — STL: iterator, thuật toán, container adapter

**Câu hỏi:** accumulate(vd, 0) với vector<double>{1.5, 2.5} bằng mấy? Sau remove(…, 3), size còn bao nhiêu? set từ {5,1,3,1,5} in gì? map in theo thứ tự nào? priority_queue.top()? stack/queue lấy phần tử nào trước?

**Output mong đợi:**

```text
accumulate(vd, 0) = 3, accumulate(vd, 0.0) = 4
find 5 o vi tri 4, count 3 = 3, count_if chan = 1
max = 5, find 9 == end ? 1
sau remove(3): size = 6, phan con lai = 3
sau erase: 4 1 5 
sort giam dan: 5 4 1 
transform x*x: 25 16 1 
reverse: 1 16 25 
set: 1 3 5 
insert 3 lan nua: inserted = 0, *pos = 3
map: Frank=11 Larry=3 Moe=2 , size = 3, m.count("Joe") = 0
deque: 1 2 3 4 
list: 1 2 3 4 
stack.top = 5, queue.front = 3, queue.back = 5, pq.top = 5
pq lay ra: 5 4 3 1 1 
```

**Giải thích:**

- `accumulate(..., 0)`: tổng có kiểu int → 0 + 1.5 → 1, 1 + 2.5 → 3. Với 0.0 thì đúng là 4.
- `find` trả về iterator; không thấy thì trả về `end()`. `it - v.begin()` là chỉ số.
- `remove` dồn các phần tử khác 3 lên đầu và trả về "end mới"; size vẫn là 6. Phải `erase(new_end, end())` (erase-remove idiom).
- `sort` với lambda so sánh `a > b` → giảm dần. `transform` có thể ghi đè lên chính container nguồn.
- set loại trùng và tự sắp xếp; insert phần tử đã có trả về `{iterator, false}`.
- map sắp theo key (chuỗi theo thứ tự từ điển); `m["Moe"]` tự thêm key mới; `count` chỉ trả về 0 hoặc 1.
- stack: LIFO (top = phần tử push cuối, 5). queue: FIFO (front = 3, back = 5). priority_queue: max-heap.

**Liên hệ đề thi:** Ba thành phần của STL: container, iterator, algorithm (thêm functor/lambda). Loại iterator: input, output, forward (`forward_list`), bidirectional (`list`, `map`, `set`), random access (`vector`, `deque`, `array`). `std::sort` cần random access nên `list` phải dùng `list::sort()`.

### 29_lambda_enum — Lambda và enum / enum class

**Câu hỏi:** Gọi by_val() sau khi x = 100 in ra gì, by_ref() thì sao? counter() mutable gọi 3 lần, biến n bên ngoài đổi không? Giá trị của Color::Blue, Status::Error? Red == 0 với enum thường và enum class?

**Output mong đợi:**

```text
by_val() = 1, by_ref() = 100
counter mutable: 1 2 3, n ngoai = 0
copy() = 4, counter() = 4
total = 21, so phan tu > 3 = 3
add5(10) = 15, square(3) = 9, square(1.5) = 2.25, IIFE = 42
Red = 0, Green = 5, Blue = 6, Red == 0 ? 1
int as_int = Blue -> 6
Status::Error = E, sizeof(Status) = 1, Color2::Blue = 2
Error
```

**Giải thích:**

- `[x]` chụp giá trị 1 lúc tạo lambda; `[&x]` đọc x lúc gọi → 100.
- `mutable` cho phép sửa bản copy n riêng của lambda; bản copy này giữ trạng thái giữa các lần gọi. Biến n bên ngoài vẫn là 0.
- `auto copy = counter;` copy cả trạng thái (n = 3) → hai lambda tăng độc lập: cả hai đều trả về 4.
- `[&total]` cộng dồn vào biến ngoài; `[threshold]` chỉ đọc.
- `make_adder` trả về lambda chụp n theo giá trị → an toàn sau khi hàm kết thúc. `[](auto a)` là generic lambda (C++14). Lambda gọi ngay khi định nghĩa (IIFE).
- enum: giá trị bắt đầu từ 0, phần tử không gán thì bằng phần tử trước + 1 → Blue = 6. Enum thường tự chuyển sang int.
- `enum class Status : char` chọn kiểu nền là char → sizeof = 1. enum class không tự chuyển sang int, phải `static_cast`.

**Liên hệ đề thi:** Cú pháp: `[capture](params) mutable noexcept -> ret { body }`. Lambda không capture gì chuyển được thành con trỏ hàm. enum class tránh trùng tên (hai enum đều có `Red`) và tránh so sánh nhầm giữa hai enum khác nhau.

## 03-process

### 01_fork_count — fork() ba lần ⚠️ không cố định

**Câu hỏi:** Có bao nhiêu dòng "PID = ..." được in? Có bao nhiêu PPID khác nhau?

**Output mong đợi:**

```text
8 dòng, thứ tự không cố định, ví dụ:
PID = 5001, PPID = 4990     <- tiến trình gốc (PPID = shell/make)
PID = 5002, PPID = 5001
PID = 5003, PPID = 5001
PID = 5005, PPID = 5001
PID = 5004, PPID = 5002
PID = 5006, PPID = 5002
PID = 5007, PPID = 5003
PID = 5008, PPID = 5004
```

**Giải thích:**

- 1 → 2 → 4 → 8 tiến trình. Cây: gốc có 3 con, con thứ nhất có 2 con, ...
- Có 5 PPID khác nhau (gốc, và 4 tiến trình có con — mọi tiến trình được tạo bởi fork thứ 1 hoặc 2 đều có con).
- Thứ tự in phụ thuộc scheduler → **không cố định**.

**Liên hệ đề thi:** Công thức: n lần `fork()` liên tiếp → 2^n tiến trình, `printf` sau đó in 2^n lần; số tiến trình **con được tạo** = 2^n − 1.

### 02_fork_if — fork() trong if và trong biểu thức logic ⚠️ không cố định

**Câu hỏi:** Phần [1] in mấy dòng? Phần [2] với fork() && fork() || fork() in mấy dòng?

**Output mong đợi:**

```text
[1] xuất hiện 3 lần
-----
[2] xuất hiện 5 lần
(PID và thứ tự dòng không cố định)
```

**Giải thích:**

- Phần 1: gốc fork ra C1 (cha nhận > 0 nên không vào if). C1 nhận 0 → fork tiếp ra C2. Tổng 3 tiến trình.
- Phần 2, đặt A, B, C là 3 lời gọi:
  - Gốc: A > 0 → chạy B (tạo P2). Ở gốc B > 0 → `&&` đúng → bỏ qua C.
  - P2: B = 0 → `&&` sai → chạy C (tạo P3).
  - P1 (con của A): A = 0 → bỏ B, chạy C (tạo P4).
  - Tổng: gốc, P1, P2, P3, P4 = **5**.

**Liên hệ đề thi:** Dạng "bao nhiêu tiến trình được tạo" xuất hiện rất thường xuyên. Vẽ cây, ghi giá trị trả về (0 hoặc >0) ở từng nhánh.

### 03_fork_buffer — Buffer của stdio khi fork

**Câu hỏi:** Màn hình hiện gì? Chữ A xuất hiện mấy lần?

**Output mong đợi:**

```text
AB
AB
```

**Giải thích:**

- `"A"` chưa có `\n` nên còn nằm trong buffer của `stdout`. `fork()` copy toàn bộ bộ nhớ user space, **kể cả buffer** → cả cha và con đều có "A" chờ in.
- Sửa: `fflush(stdout)` trước `fork()` → "A" in 1 lần: `AB` rồi `B`.
- Nếu là `printf("A\n")`: ra terminal in `A, B, B`; nhưng chuyển hướng ra file (fully buffered) lại thành `A B A B`.
- `write(1, ...)` là system call, không có buffer user space → không bị nhân đôi.

**Liên hệ đề thi:** Câu hỏi "tại sao output bị lặp khi chuyển hướng ra file" → do buffer stdio bị nhân bản qua `fork`. Nhớ: `exit()` flush buffer, `_exit()` thì không.

### 04_fork_exec_wait — fork + exec + waitpid

**Câu hỏi:** Mã thoát (WEXITSTATUS) mà cha in ra là bao nhiêu? Dòng perror("execlp") có chạy không?

**Output mong đợi:**

```text
Con (pid 6120) se chay ls
ls: cannot access '/khong_ton_tai': No such file or directory
Cha: con 6120 thoat voi ma 2
```

**Giải thích:**

- `execlp` thành công (tìm thấy `ls` trong PATH) → tiến trình con trở thành `ls`, các dòng sau `execlp` **không bao giờ chạy**.
- `ls` thoát với mã 2 khi có lỗi nghiêm trọng (không truy cập được đối số). Thư mục hợp lệ → 0.
- Nếu đổi thành `lsx`: exec thất bại → `perror` in `execlp: No such file or directory` → mã 127 (quy ước của shell cho "command not found").
- Mã thoát chỉ lấy 8 bit thấp: `exit(256)` → cha thấy 0.

**Liên hệ đề thi:** Ý nghĩa hậu tố: **l** = danh sách đối số, **v** = mảng `argv`, **p** = tìm trong PATH, **e** = truyền môi trường. Tất cả cuối cùng gọi system call `execve`.

### 05_zombie — Tiến trình zombie

**Câu hỏi:** Trong 30 giây đó, ở terminal thứ 2, cột STAT của tiến trình con hiện chữ gì? Tên lệnh của nó hiển thị thế nào?

**Output mong đợi:**

```text
Terminal 1:
Cha 7001: ngu 30s, KHONG goi wait(). Con = 7002
Terminal 2: ps -o pid,ppid,stat,cmd --ppid 7001
Con 7002: thoat ngay
(sau 30 giây) Cha thoat

Terminal 2:
    PID    PPID STAT CMD
   7002    7001 Z+   [05_zombie] <defunct>
```

**Giải thích:**

- STAT `Z` = zombie, tên lệnh có `<defunct>`. Zombie không tốn CPU/RAM nhưng giữ **PID** — quá nhiều sẽ cạn PID.
- `kill -9` zombie **không có tác dụng**: nó đã chết rồi.
- Khi cha thoát, zombie thành orphan → được `init/systemd` (hoặc subreaper) nhận nuôi và tự gọi `wait` dọn dẹp.
- Cách tránh: gọi `wait/waitpid`, xử lý `SIGCHLD`, hoặc `signal(SIGCHLD, SIG_IGN)`.

**Liên hệ đề thi:** Trạng thái tiến trình trong `ps`: R (running), S (sleep, chờ ngắt được), D (sleep không ngắt được, thường chờ I/O), T (stopped), Z (zombie).

### 06_orphan — Tiến trình mồ côi (orphan) ⚠️ không cố định

**Câu hỏi:** PPID mà con in ra ở dòng "truoc" và dòng "sau" khác nhau thế nào? Dấu nhắc shell xuất hiện ở đâu?

**Output mong đợi:**

```text
Cha 8100: thoat sau 1s
Con 8101: truoc, PPID = 8100
user@ubuntu:~/bosch-lab$ Con 8101: sau,   PPID = 1      <- hoặc PID của "systemd --user"
```

**Giải thích:**

- Lúc đầu PPID là PID của cha. Cha thoát sau 1 giây → con thành orphan, được nhận nuôi.
- Trên Ubuntu desktop thường thấy PPID là PID của `systemd --user` (một **subreaper**), không nhất thiết là 1. Trong Docker/WSL có thể là 1.
- Shell chỉ chờ tiến trình cha, nên dấu nhắc hiện ra **trước** dòng "sau" của con.

**Liên hệ đề thi:** Zombie: con chết, cha còn sống nhưng không wait. Orphan: cha chết, con còn sống. Orphan không có hại; zombie tích tụ thì có hại.

### 07_fork_cow — Biến sau fork(): copy-on-write

**Câu hỏi:** Cha in g, local, heap bằng mấy sau khi con đã tăng cả ba? Địa chỉ &g ở cha và con có giống nhau không?

**Output mong đợi:**

```text
con: g=11 local=21 heap=31 | &g=0x5870e837b010 heap=0x587110e802a0
cha: g=10 local=20 heap=30 | &g=0x5870e837b010 heap=0x587110e802a0
```

**Giải thích:**

- Con sửa bản sao của nó; cha vẫn thấy giá trị cũ 10/20/30.
- Địa chỉ in ra **giống hệt** vì đó là địa chỉ ảo — con có bảng trang riêng trỏ tới trang vật lý riêng. Số địa chỉ cụ thể khác nhau mỗi lần chạy (ASLR), nhưng cha và con luôn bằng nhau.
- Copy-on-write: sau fork, hai tiến trình dùng chung trang vật lý ở chế độ chỉ đọc; khi một bên ghi, kernel mới copy trang đó. Nhờ vậy fork rất nhanh.

**Liên hệ đề thi:** Con kế thừa: bản sao bộ nhớ, fd đang mở, signal handler, cwd, biến môi trường. Con KHÔNG kế thừa: PID, signal đang pending, lock của file (fcntl), timer (alarm), các thread khác.

### 08_fork_fd_offset — File descriptor sau fork(): dùng chung offset

**Câu hỏi:** File 1 chứa gì (cha và con dùng chung fd mở TRƯỚC fork)? File 2 chứa gì (mỗi bên tự open SAU fork)?

**Output mong đợi:**

```text
/tmp/bosch_offset1.txt (8 byte):
con
cha
/tmp/bosch_offset2.txt (9 byte):
cha
XXXX
```

**Giải thích:**

- File 1: fd được nhân bản nhưng cùng trỏ một open file description → offset chung. Con ghi 4 byte, offset thành 4, cha ghi tiếp từ 4 → "con\ncha\n".
- File 2: mỗi bên `open` riêng → offset riêng bắt đầu từ 0. Cha ghi đè "cha\n" lên 4 byte đầu của "XXXXXXXX\n".

**Liên hệ đề thi:** Ba tầng: bảng fd (riêng mỗi tiến trình) → open file table (offset, cờ; dùng chung sau fork/dup) → inode. `dup2(fd, 1)` là cách shell làm redirect.

### 09_exec_buffer — exec() thay thế chương trình: PID giữ nguyên, buffer stdio mất

**Câu hỏi:** Dòng "chua xuong dong..." có xuất hiện không? PID sau exec có đổi không? Dòng "khong bao gio in" có in không?

**Output mong đợi:**

```text
PID truoc exec = 10144
PID sau exec  = 10144
```

**Giải thích:**

- Dòng đầu có `\n` và stdout là terminal (line-buffered) → đã flush trước exec.
- "chua xuong dong..." còn nằm trong buffer của stdio (bộ nhớ user space) → exec xoá toàn bộ bộ nhớ cũ → mất.
- `$$` trong `sh -c` là PID của sh = PID cũ, vì exec không tạo tiến trình mới. Số PID cụ thể khác mỗi lần chạy.
- exec thành công không bao giờ trả về → hai dòng cuối không chạy.

**Liên hệ đề thi:** fork tạo tiến trình mới (bản sao), exec thay ruột tiến trình hiện tại. Shell chạy lệnh = fork + exec ở con + wait ở cha. `vfork`/`posix_spawn` tối ưu cho trường hợp exec ngay sau fork.

## 04-signal

### 01_sigint_handler — Bắt Ctrl+C bằng sigaction

**Câu hỏi:** Nhấn Ctrl+C lần 1, 2 thì sao? Lần thứ mấy chương trình thoát, và dòng cuối in gì?

**Output mong đợi:**

```text
PID 9001. Nhan Ctrl+C 3 lan de thoat.
dang chay... count = 0
dang chay... count = 0
^C [handler: nhan SIGINT]
dang chay... count = 1
^C [handler: nhan SIGINT]
dang chay... count = 2
^C [handler: nhan SIGINT]
Thoat sach se sau 3 lan Ctrl+C
```

**Giải thích:**

- Ctrl+C khiến terminal gửi **SIGINT** (2) tới cả foreground process group. Handler đã thay thế hành động mặc định (terminate).
- `sleep()` bị signal làm gián đoạn và trả về sớm, nên vòng lặp kiểm tra `stop` ngay.
- `volatile`: buộc đọc lại biến từ bộ nhớ mỗi lần (không cache trong thanh ghi). `sig_atomic_t`: đọc/ghi nguyên tử với signal.
- `printf` không async-signal-safe (dùng lock + buffer nội bộ) → dùng `write` trong handler.

**Liên hệ đề thi:** `signal()` cũ có ngữ nghĩa khác nhau giữa các hệ; `sigaction()` là chuẩn POSIX, cho phép đặt `sa_mask`, `SA_RESTART`, `SA_SIGINFO`.

### 02_sigchld_reap — Dọn zombie bằng SIGCHLD ⚠️ không cố định

**Câu hỏi:** Chương trình có in đủ "Da don du 5 con" không? Nếu handler chỉ gọi waitpid một lần (không có while) thì có thể sai ở đâu?

**Output mong đợi:**

```text
(số dòng giữa có thể khác nhau mỗi lần chạy)
cha dang lam viec... da don 0 con
cha dang lam viec... da don 1 con
cha dang lam viec... da don 2 con
cha dang lam viec... da don 3 con
cha dang lam viec... da don 4 con
Da don du 5 con, khong con zombie
```

**Giải thích:**

- Mỗi lần SIGCHLD đến, `sleep` bị cắt ngang nên cha in lại tiến độ.
- `WNOHANG`: không có con nào đã thoát thì trả về 0 ngay (không chặn). `-1` = bất kỳ con nào.
- Nếu chỉ `waitpid` một lần: khi 2 con thoát gần như đồng thời, SIGCHLD thứ hai bị gộp (pending chỉ là 1 bit) → một con thành zombie và `reaped` không bao giờ tới 5 → treo.
- Lưu/khôi phục `errno` vì handler có thể chen vào giữa code đang kiểm tra `errno`.

**Liên hệ đề thi:** Standard signals không queue; real-time signals (`SIGRTMIN..SIGRTMAX`) thì có queue. Mặc định hành động của SIGCHLD là **ignore**.

### 03_kill_test — Gửi signal bằng lệnh kill

**Câu hỏi:** Dòng đầu tiên in gì (sigaction cho SIGKILL)? Với từng lệnh kill -TERM, -USR1, -STOP, -CONT, -KILL từ terminal 2: chương trình phản ứng ra sao?

**Output mong đợi:**

```text
sigaction(SIGKILL): Invalid argument
PID = 9500. Terminal 2 thu lan luot:
  ...
tick 0
tick 1
[handler] SIGTERM -> van chay tiep     <- kill -TERM
tick 2
[handler] SIGUSR1                      <- kill -USR1
tick 3
                                       <- kill -STOP: ngừng in tick (ps thấy STAT = T)
tick 4                                 <- kill -CONT: chạy tiếp
Killed                                 <- kill -KILL: chết ngay, shell in "Killed"
```

**Giải thích:**

- `sigaction(SIGKILL, ...)` trả về -1, `errno = EINVAL` → in `Invalid argument`.
- SIGTERM bị bắt nên tiến trình **không** chết. Đây là lý do `kill <pid>` (mặc định là TERM) đôi khi không giết được tiến trình.
- SIGSTOP dừng tiến trình (không bắt được), SIGCONT cho chạy tiếp.
- SIGKILL do kernel xử lý trực tiếp, chương trình không có cơ hội dọn dẹp.

**Liên hệ đề thi:** Thuộc số hiệu trên Linux x86: HUP 1, INT 2, QUIT 3, ABRT 6, KILL 9, USR1 10, SEGV 11, USR2 12, PIPE 13, ALRM 14, TERM 15, CHLD 17, CONT 18, STOP 19, TSTP 20.

### 04_alarm_pause — alarm, pause và hành động mặc định của SIGALRM

**Câu hỏi:** alarm(0) trả về mấy? pause() trả về gì và errno là gì? Dòng cuối "khong in" có xuất hiện không, và shell báo gì?

**Output mong đợi:**

```text
alarm(0) tra ve 10
alarm(1) roi pause()...
  [handler] SIGALRM
pause() = -1, errno = Interrupted system call
alarm(1) lan nua, lan nay KHONG bat SIGALRM
Alarm clock
```

**Giải thích:**

- `alarm(0)` huỷ hẹn giờ đang có và trả số giây còn lại (10).
- Sau 1 giây, SIGALRM tới → handler chạy → pause() trả -1 với `errno = EINTR`.
- Lần hai không bắt SIGALRM → hành động mặc định là **kết thúc** tiến trình. Shell in "Alarm clock" và `echo $?` = 142 (128 + 14). "khong in" không xuất hiện.

**Liên hệ đề thi:** Hành động mặc định: Term (SIGINT, SIGTERM, SIGALRM, SIGPIPE, SIGUSR1/2, SIGHUP, SIGKILL), Core (SIGSEGV, SIGABRT, SIGFPE, SIGQUIT), Ign (SIGCHLD), Stop (SIGSTOP, SIGTSTP), Cont (SIGCONT). Mã thoát khi chết vì signal n là 128 + n.

### 05_sigprocmask — Chặn signal (sigprocmask), signal đang chờ và việc signal không xếp hàng

**Câu hỏi:** Gửi SIGUSR1 3 lần khi đang chặn → bỏ chặn thì handler chạy mấy lần? Còn với signal real-time SIGRTMIN?

**Output mong đợi:**

```text
dang chan: SIGUSR1 pending? 1, SIGRTMIN pending? 1
truoc khi bo chan: usr1 = 0, rt = 0
sau khi bo chan:   usr1 = 1, rt = 3
```

**Giải thích:**

- Khi bị chặn, signal không được giao mà chuyển sang pending → handler chưa chạy (0, 0).
- Signal chuẩn chỉ có một bit pending → 3 lần raise gộp thành 1.
- Signal real-time được xếp hàng → giao đủ 3 lần, và mang theo dữ liệu được (`sigqueue`).
- Signal đang pending được giao ngay khi `sigprocmask(SIG_UNBLOCK)` trả về.

**Liên hệ đề thi:** Phân biệt **chặn** (block: hoãn lại, vẫn pending) và **bỏ qua** (ignore: vứt bỏ). SIGKILL và SIGSTOP không thể chặn, bỏ qua hay bắt. Trong thread dùng `pthread_sigmask`.

## 05-thread

### 01_race_condition — Race condition với counter++ ⚠️ không cố định

**Câu hỏi:** Giá trị counter in ra có bằng 2000000 không? Chạy 3 lần có giống nhau không?

**Output mong đợi:**

```text
counter = 1087342 (mong doi 2000000)     <- số nhỏ hơn 2000000, mỗi lần chạy một khác
```

**Giải thích:**

- Thread 1 đọc counter = 5, thread 2 cũng đọc 5, cả hai ghi 6 → mất một lần tăng (lost update).
- Kết quả **không cố định**; trên máy 1 nhân hoặc N nhỏ có thể tình cờ đúng — race condition vẫn tồn tại.
- `volatile` **không** sửa được lỗi này (chỉ ngăn tối ưu hoá, không tạo tính nguyên tử).

**Liên hệ đề thi:** Ba cách sửa: mutex, biến atomic (`__atomic_fetch_add`, `stdatomic.h`, `std::atomic`), hoặc mỗi thread đếm riêng rồi cộng sau khi join.

### 02_mutex_fix — Sửa race bằng pthread_mutex

**Câu hỏi:** counter giờ bằng bao nhiêu? Chạy chậm hơn hay nhanh hơn bài 01?

**Output mong đợi:**

```text
counter = 2000000 (mong doi 2000000)
```

**Giải thích:**

- Luôn đúng 2000000. Chạy **chậm hơn** rõ rệt vì 2 triệu lần lock/unlock và tranh chấp giữa các nhân CPU.
- Linux hiện thực mutex bằng **futex**: không tranh chấp thì lock hoàn toàn ở user space; có tranh chấp mới gọi vào kernel.
- Giữ lock càng ngắn càng tốt, nhưng lock quá vụn cũng tốn chi phí.

**Liên hệ đề thi:** Mutex có **ownership**: thread nào lock thì thread đó unlock. Lock 2 lần cùng một mutex thường (default) trong 1 thread → deadlock; dùng mutex `RECURSIVE` nếu cần.

### 03_deadlock — Deadlock do khoá ngược thứ tự

**Câu hỏi:** Chương trình in những dòng nào rồi sao nữa? Dòng "Khong deadlock" có xuất hiện không?

**Output mong đợi:**

```text
T1: giu A, cho B...
T2: giu B, cho A...
(đứng yên mãi mãi — Ctrl+C để thoát; thứ tự 2 dòng có thể đảo)
```

**Giải thích:**

- Đủ cả 4 điều kiện Coffman: **mutual exclusion**, **hold and wait**, **no preemption**, **circular wait**.
- `usleep` chỉ làm deadlock xảy ra gần như chắc chắn; bỏ đi thì vẫn có thể deadlock, chỉ là thỉnh thoảng.
- Debug: `gdb -p <PID>` rồi `thread apply all bt` thấy cả 2 thread kẹt ở `__lll_lock_wait`.
- Sửa: khoá theo thứ tự cố định (phá circular wait), dùng `trylock` + back-off, hoặc `std::scoped_lock(a, b)` trong C++17.

**Liên hệ đề thi:** Livelock: các thread vẫn chạy nhưng liên tục nhường nhau, không tiến triển. Starvation: một thread không bao giờ được cấp tài nguyên.

### 04_cond_var — Producer–consumer một slot với condition variable

**Câu hỏi:** Thứ tự các dòng produce / consume? Có thể có 2 dòng produce liền nhau không?

**Output mong đợi:**

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

**Giải thích:**

- Slot chỉ chứa 1 phần tử: producer phải chờ `full == 0`, consumer chờ `full == 1` → luân phiên tuyệt đối, **không thể** có 2 dòng produce liền nhau.
- `pthread_cond_signal` không có "bộ nhớ": nếu không ai đang chờ thì tín hiệu mất. Vì vậy trạng thái thật nằm ở biến `full`, được bảo vệ bởi mutex.
- Dùng `while` vì có thể bị đánh thức giả (spurious wakeup) hoặc thread khác đã lấy mất dữ liệu.

**Liên hệ đề thi:** Condition variable luôn đi kèm 1 mutex + 1 điều kiện (predicate). `signal` đánh thức ít nhất 1 thread, `broadcast` đánh thức tất cả.

### 05_semaphore — Semaphore giới hạn 2 thread cùng lúc ⚠️ không cố định

**Câu hỏi:** Giá trị "toi da cung luc" là bao nhiêu? Thứ tự vào/ra có cố định không?

**Output mong đợi:**

```text
(thứ tự thay đổi, ví dụ)
T0 vao (dang trong: 1)
T1 vao (dang trong: 2)
T0 ra
T2 vao (dang trong: 2)
T1 ra
T3 vao (dang trong: 2)
...
So thread toi da cung luc = 2
```

**Giải thích:**

- Không bao giờ quá 2 thread ở giữa `sem_wait` và `sem_post` → max = 2.
- Mutex `m` riêng chỉ để bảo vệ biến đếm `inside` và `printf`.
- Tham số thứ 2 của `sem_init` = 0: chia sẻ giữa các thread; ≠ 0: chia sẻ giữa các process (semaphore phải nằm trong shared memory).

**Liên hệ đề thi:** Mutex vs semaphore: mutex có chủ sở hữu, dùng cho loại trừ tương hỗ; semaphore là bộ đếm, thread khác có thể `post`, dùng để báo hiệu/giới hạn tài nguyên. Named semaphore: `sem_open("/ten", ...)`.

### 06_cpp_thread — std::thread, std::mutex, lock_guard, std::atomic ⚠️ không cố định

**Câu hỏi:** plain, guarded, atomic lần lượt bằng bao nhiêu? Cái nào có thể sai?

**Output mong đợi:**

```text
plain   = 1203117      <- sai, thay đổi mỗi lần chạy
guarded = 2000000
atomic  = 2000000
so nhan CPU = 8        <- tuỳ máy
```

**Giải thích:**

- `plain` là race condition giống bài 01.
- `lock_guard` khoá trong constructor, mở khoá trong destructor → không thể quên unlock, kể cả khi có exception.
- `atomic<int>` dùng lệnh CPU nguyên tử (x86: `lock xadd`), thường nhanh hơn mutex cho thao tác đơn giản.
- Một `std::thread` còn **joinable** mà bị huỷ → gọi `std::terminate()` → chương trình abort.

**Liên hệ đề thi:** `unique_lock` linh hoạt hơn `lock_guard` (unlock giữa chừng, dùng với `condition_variable`). C++20 có `std::jthread` tự join khi huỷ.

### 07_thread_arg_bug — Truyền &i vào thread và lấy kết quả qua pthread_join ⚠️ không cố định

**Câu hỏi:** Phần "SAI" in id nào? Phần "DUNG" in gì và tổng bằng bao nhiêu?

**Output mong đợi:**

```text
  SAI: thread nhan id = 2
  SAI: thread nhan id = 3
  SAI: thread nhan id = 4
  SAI: thread nhan id = 2
  DUNG: thread 0 tra ve 0
  DUNG: thread 1 tra ve 1
  DUNG: thread 2 tra ve 4
  DUNG: thread 3 tra ve 9
tong = 14
```

**Giải thích:**

- Phần SAI: các thread đọc `*arg` vào lúc nào đó **sau** khi được tạo; lúc đó main có thể đã tăng i, thậm chí đã ra khỏi vòng lặp (giá trị 4, hoặc ô nhớ đã bị vòng lặp sau dùng lại). Mỗi lần chạy một kiểu, thứ tự dòng cũng thay đổi.
- Phần DUNG: id được truyền **theo giá trị** trong chính con trỏ → mỗi thread có bản riêng. Kết quả lấy qua `pthread_join(t, &ret)` theo thứ tự join → cố định: 0 + 1 + 4 + 9 = 14.

**Liên hệ đề thi:** Lỗi truyền `&i` vào thread hay xuất hiện trong câu "tìm bug". Thread trả về con trỏ tới biến local của nó là sai (stack đã bị huỷ) — trả malloc hoặc giá trị ép kiểu.

### 08_thread_local — Thread chia sẻ gì, riêng gì: global, thread-local, PID/TID

**Câu hỏi:** Mỗi thread in mine và shared bằng mấy? Cuối cùng main thấy mine, shared bằng mấy? PID của thread có bằng PID của main không?

**Output mong đợi:**

```text
thread 0: mine = 3, shared = 3, getpid()==main? 1, tid==pid? 0
thread 1: mine = 3, shared = 6, getpid()==main? 1, tid==pid? 0
main: mine = 0, shared = 6, tid==pid? 1
```

**Giải thích:**

- `shared` là biến toàn cục dùng chung → cộng dồn 3 rồi 6.
- `mine` là `_Thread_local` → mỗi thread một bản bắt đầu từ 0; bản của main không bị đụng tới → 0.
- `getpid()` giống nhau ở mọi thread (TGID). TID của thread phụ khác PID; chỉ thread chính có TID = PID.

**Liên hệ đề thi:** Thread chia sẻ: code, data, heap, fd, signal handler, cwd, PID. Riêng: stack, thanh ghi/PC, TID, signal mask, errno (errno là thread-local). Trên Linux cả process và thread đều tạo bằng `clone()` với các cờ chia sẻ khác nhau.

## 06-ipc

### 01_pipe — pipe: cha ghi, con đọc

**Câu hỏi:** Con đọc được bao nhiêu byte? read trả về bao nhiêu ở lần cuối? Nếu con KHÔNG đóng fd[1] thì sao?

**Output mong đợi:**

```text
Con doc 15 byte: xin chao tu cha
Con: read tra ve 0
Cha: xong

Nếu con không close(fd[1]): sau dòng đầu, chương trình TREO (con chờ read mãi).
```

**Giải thích:**

- `"xin chao tu cha"` dài 15 ký tự; `strlen` không tính `'\0'`.
- Sau khi cha đóng đầu ghi, `read` trả về 0 = EOF → thoát vòng lặp.
- Nếu con vẫn giữ `fd[1]`, kernel thấy vẫn còn một "writer" (chính nó) nên `read` chặn mãi → deadlock tự gây ra.
- Ghi vào pipe không còn đầu đọc → nhận **SIGPIPE** (mặc định chết), hoặc `write` trả về `EPIPE` nếu bỏ qua signal.

**Liên hệ đề thi:** Pipe là một chiều, chỉ dùng giữa các tiến trình có quan hệ họ hàng (kế thừa fd qua fork). Ghi ≤ `PIPE_BUF` (4096 byte trên Linux) là nguyên tử. Dung lượng mặc định 64 KiB.

### 02_pipe_dup2 — Mô phỏng ls / | wc -l bằng dup2

**Câu hỏi:** Chương trình in ra số nào (so với chạy ls / | wc -l trong shell)? Nếu cha quên đóng fd[1] thì sao?

**Output mong đợi:**

```text
22                                  <- đúng bằng kết quả của "ls / | wc -l" trên máy bạn
Xong: tuong duong  ls / | wc -l

Nếu cha không đóng fd[1]: wc không bao giờ thấy EOF → treo.
```

**Giải thích:**

- Con 1: stdout (fd 1) → đầu ghi của pipe, rồi `exec ls`. Con 2: stdin (fd 0) ← đầu đọc, rồi `exec wc -l`.
- File descriptor được giữ nguyên qua `exec`, nên `ls` và `wc` không hề biết mình đang dùng pipe.
- Mọi bản sao của đầu ghi phải đóng thì `wc` mới nhận EOF: con 1 (sau khi `ls` thoát), con 2 và **cả cha**.

**Liên hệ đề thi:** `dup(fd)` trả về fd nhỏ nhất còn trống. `2>&1` trong shell chính là `dup2(1, 2)`. Thứ tự redirect trong shell quan trọng vì nó là chuỗi lời gọi dup2 tuần tự.

### 03_fifo — Named pipe (FIFO) giữa 2 chương trình

**Câu hỏi:** Khi chỉ chạy reader, nó dừng ở dòng nào? Khi writer gõ Ctrl+D, reader in gì rồi kết thúc ra sao?

**Output mong đợi:**

```text
Terminal 1 (reader, chạy trước):
Reader: dang mo FIFO...                 <- dừng ở đây cho tới khi writer mở
Reader: da mo, cho du lieu
Reader nhan: xin chao
Reader nhan: bosch
Reader: read tra ve 0, ket thuc

Terminal 2 (writer):
Writer: dang mo FIFO...
Writer: da mo. Go vai dong roi Ctrl+D de ket thuc
xin chao
bosch
^D
Writer: da dong FIFO
```

**Giải thích:**

- `open` đầu đọc chặn tới khi có đầu ghi (và ngược lại) — đây là hành vi mặc định khi không có `O_NONBLOCK`.
- Writer đóng fd → reader nhận EOF (read = 0).
- `ls -l` cho thấy ký tự đầu là `p` (pipe). Dữ liệu không ghi xuống đĩa; file chỉ là "điểm hẹn" trong filesystem.

**Liên hệ đề thi:** Ký tự loại file trong `ls -l`: `-` file thường, `d` thư mục, `l` symlink, `c` char device, `b` block device, `p` FIFO, `s` socket.

### 04_shm — POSIX shared memory giữa cha và con

**Câu hỏi:** Cha đọc được chuỗi nào trong mem? Biến local ở cha bằng mấy?

**Output mong đợi:**

```text
Con: mem = "con da ghi vao shm", local = 99
Cha: mem = "con da ghi vao shm", local = 1
```

**Giải thích:**

- Shared memory là IPC **nhanh nhất**: sau khi map, đọc/ghi như bộ nhớ thường, không qua system call → nhưng phải tự đồng bộ (semaphore/mutex có `PTHREAD_PROCESS_SHARED`).
- `local` nằm ở vùng nhớ riêng, sau `fork` là copy-on-write → cha vẫn thấy 1.
- Đối tượng POSIX shm nằm trong `/dev/shm` (tmpfs). `close(fd)` không huỷ mapping; phải `munmap` và `shm_unlink`.
- Ubuntu cũ (glibc < 2.34) cần link thêm `-lrt`.

**Liên hệ đề thi:** System V: `shmget / shmat / shmdt / shmctl(IPC_RMID)`, xem bằng `ipcs -m`. POSIX: `shm_open / ftruncate / mmap / munmap / shm_unlink`.

### 05_mmap_file — Ánh xạ file vào bộ nhớ bằng mmap

**Câu hỏi:** Dòng "Sau" in gì? Kích thước file là bao nhiêu byte?

**Output mong đợi:**

```text
Truoc: hello mmap world
Sau  : HELLO MMAP world
File size = 17 byte
```

**Giải thích:**

- 16 ký tự chữ + `'\n'` = 17 byte.
- Ghi trực tiếp vào vùng nhớ đã map làm thay đổi file vì dùng `MAP_SHARED`. Với `MAP_PRIVATE`, thay đổi chỉ nằm trong bản copy-on-write riêng, file không đổi.
- `mmap` làm việc theo **trang** (4096 byte): truy cập ngoài file nhưng trong trang cuối → đọc ra 0, không lưu vào file; vượt trang → **SIGBUS/SIGSEGV**.

**Liên hệ đề thi:** Ưu điểm mmap: tránh copy giữa kernel buffer và user buffer, truy cập ngẫu nhiên tiện lợi. `MAP_ANONYMOUS` tạo vùng nhớ không gắn file (malloc dùng cách này cho khối lớn).

### 06_unix_socket — UNIX domain socket: server/client bằng fork

**Câu hỏi:** Hai dòng in ra theo thứ tự nào? Vì sao bind/listen phải làm TRƯỚC fork?

**Output mong đợi:**

```text
Server nhan: ping
Client nhan: pong
```

**Giải thích:**

- Client chỉ in sau khi nhận "pong", mà server chỉ gửi "pong" sau khi đã in → thứ tự cố định.
- Nếu fork trước rồi mới bind/listen, client có thể `connect` khi socket chưa listen → `ECONNREFUSED`/`ENOENT`.
- File socket còn tồn tại sau khi chương trình thoát → `bind` lần sau lỗi **Address already in use** nếu không `unlink`.
- `socketpair(AF_UNIX, ...)` tạo sẵn một cặp socket đã nối, tiện cho cha–con.

**Liên hệ đề thi:** `SOCK_STREAM` (TCP-like, tin cậy, có thứ tự, dạng luồng byte) vs `SOCK_DGRAM` (UDP-like, giữ ranh giới message). UNIX socket còn truyền được file descriptor (SCM_RIGHTS).

### 07_mqueue — POSIX message queue và độ ưu tiên

**Câu hỏi:** Con gửi 3 tin với độ ưu tiên 1, 5, 3. Cha nhận theo thứ tự nào? mq_curmsgs trước khi nhận bằng mấy?

**Output mong đợi:**

```text
so tin dang cho: 3
nhan "cao" (prio 5)
nhan "vua" (prio 3)
nhan "thap" (prio 1)
```

**Giải thích:**

- Con đã thoát nhưng 3 tin vẫn còn trong queue vì queue là object của kernel, không phụ thuộc tiến trình gửi.
- `mq_receive` luôn lấy tin có ưu tiên cao nhất → 5, 3, 1.
- Mỗi lần receive lấy đúng một tin nguyên vẹn (giữ ranh giới tin), khác pipe là luồng byte liên tục.

**Liên hệ đề thi:** POSIX mq (`mq_open`, tên bắt đầu bằng `/`, link với `-lrt` trên glibc cũ) và System V mq (`msgget`, `msgsnd`, `msgrcv`, chọn tin theo `mtype`). Xem bằng `ls /dev/mqueue` và `ipcs -q`.

### 08_sigpipe — Ghi vào pipe không còn đầu đọc: SIGPIPE và EPIPE

**Câu hỏi:** Khi bỏ qua SIGPIPE, write trả về gì? Khi để mặc định, tiến trình con kết thúc thế nào? Dung lượng pipe mặc định trên Linux?

**Output mong đợi:**

```text
dung luong pipe = 65536 byte
bo qua SIGPIPE: write = -1, errno = Broken pipe
con bi kill boi signal 13 (Broken pipe)
read lan 1 = 2, read lan 2 = 0 (EOF)
```

**Giải thích:**

- Pipe trên Linux mặc định 64 KiB; đầy thì write chặn (hoặc trả EAGAIN nếu O_NONBLOCK).
- Đầu đọc đã đóng: với SIGPIPE bị bỏ qua, write trả -1/EPIPE.
- Con đặt lại SIG_DFL → nhận SIGPIPE (13) và chết ngay trong write, dòng "khong in" không chạy.
- Đọc sau khi mọi đầu ghi đã đóng: lấy hết dữ liệu còn lại (2 byte), lần sau trả 0 = EOF.

**Liên hệ đề thi:** Server mạng thường `signal(SIGPIPE, SIG_IGN)` hoặc dùng `send(..., MSG_NOSIGNAL)` để không chết khi client ngắt kết nối. Luôn đóng đầu pipe không dùng sau fork.

## 07-kernel-module

### hello — Kernel module Hello

**Câu hỏi:** Sau insmod, dmesg | tail in gì? Với sudo insmod hello.ko count=3 thì sao? rmmod in gì?

**Output mong đợi:**

```text
$ sudo apt install build-essential linux-headers-$(uname -r)
$ make
$ sudo insmod hello.ko
$ dmesg | tail -2
[12345.678901] hello: xin chao kernel (0)
$ lsmod | grep hello
hello                  16384  0
$ sudo rmmod hello
$ dmesg | tail -1
[12350.123456] hello: tam biet kernel

$ sudo insmod hello.ko count=3   -> in 3 dòng (0), (1), (2)

Nếu Secure Boot bật: insmod: ERROR: could not insert module hello.ko: Key was rejected by service
(chỉ cần build được hello.ko là đủ cho mục tiêu ôn thi)
```

**Giải thích:**

- `module_init/module_exit` khai báo điểm vào/ra. `__init` cho phép kernel giải phóng code init sau khi nạp xong.
- `MODULE_LICENSE("GPL")` thiếu sẽ làm kernel bị "tainted" và không dùng được các symbol `EXPORT_SYMBOL_GPL`.
- `insmod` nạp đúng file chỉ định; `modprobe` tìm trong `/lib/modules` và tự nạp module phụ thuộc.
- Init trả về số âm (vd `-ENOMEM`) → insmod thất bại, module không được nạp.

**Liên hệ đề thi:** Hay hỏi: lệnh xem module đang nạp (`lsmod` hay `cat /proc/modules`), xem log kernel (`dmesg`), đuôi file module (`.ko`), hàm in log trong kernel (`printk`).

## 08-shell

### Drill 01

Tạo file `run.sh`, đặt quyền `754`, kiểm tra bằng `ls -l` và `stat`. Chuỗi quyền hiển thị là gì? Ai được làm gì?

```sh
touch run.sh
chmod 754 run.sh
ls -l run.sh                 # -rwxr-xr-- 1 user user 0 ... run.sh
stat -c '%a %A %U' run.sh    # 754 -rwxr-xr-- user
```
7 = rwx (owner), 5 = r-x (group), 4 = r-- (others). r = 4, w = 2, x = 1.

### Drill 02

Xem `umask` hiện tại. Với `umask 022`, file mới tạo bằng `touch` và thư mục mới tạo bằng `mkdir` có quyền gì?

```sh
umask            # Ubuntu user thường: 0002; root: 0022
umask 022
touch f; mkdir d
ls -ld f d       # -rw-r--r-- f  (644)   drwxr-xr-x d  (755)
```
File mặc định 666, thư mục 777, rồi **bỏ** các bit có trong umask: 666 & ~022 = 644, 777 & ~022 = 755.

### Drill 03

Tạo `a.txt`, tạo hard link `hard.txt` và soft link `soft.txt` tới nó. Xoá `a.txt`. `cat` hai link cho kết quả gì? `ls -li` cho thấy gì về inode?

```sh
echo hello > a.txt
ln a.txt hard.txt
ln -s a.txt soft.txt
ls -li            # a.txt và hard.txt CÙNG số inode, link count = 2; soft.txt inode khác
rm a.txt
cat hard.txt      # hello  (dữ liệu còn vì inode còn 1 link)
cat soft.txt      # cat: soft.txt: No such file or directory  (dangling symlink)
```
Hard link = thêm một tên cho cùng inode (không tạo cho thư mục, không qua filesystem khác). Soft link = file chứa đường dẫn.

### Drill 04

Dùng `ls /etc/hostname /khong_co` (vừa in stdout vừa in stderr). So sánh `cmd > f 2>&1` với `cmd 2>&1 > f`: lỗi xuất hiện ở đâu?

```sh
ls /etc/hostname /khong_co > f1 2>&1
cat f1            # cả dòng lỗi và /etc/hostname đều nằm trong f1; màn hình trống
ls /etc/hostname /khong_co 2>&1 > f2
                  # màn hình: ls: cannot access '/khong_co': No such file or directory
cat f2            # chỉ có /etc/hostname
```
Redirect xử lý **trái sang phải**. `2>&1` nghĩa là "fd 2 trỏ tới nơi fd 1 đang trỏ **lúc này**". Ở cách 2, lúc đó fd 1 vẫn là terminal.

### Drill 05

Phân biệt `>`, `>>`, `2>/dev/null`, `&>` và `tee`. Ghi output của `date` vào `log.txt` đồng thời vẫn thấy trên màn hình, và không ghi đè nội dung cũ.

```sh
date > log.txt        # ghi đè
date >> log.txt       # nối thêm
find / -name passwd 2>/dev/null   # bỏ thông báo lỗi
ls /x /etc &> all.txt # stdout + stderr vào cùng file (bash)
date | tee -a log.txt # vừa hiện màn hình vừa nối vào file
```

### Drill 06

Trong thư mục `bosch-lab`, tìm mọi dòng chứa `fork` trong các file `.c`, hiện số dòng. Sau đó: đếm số file chứa `pthread`, tìm không phân biệt hoa thường, và in các dòng KHÔNG chứa `#include` của một file.

```sh
grep -rn "fork" --include="*.c" .
grep -rl "pthread" . | wc -l
grep -rni "mutex" .
grep -v "#include" 05-thread/02_mutex_fix.c
grep -c "printf" 01-c/05_bitwise.c      # số DÒNG khớp
```
`-r` đệ quy, `-n` số dòng, `-i` bỏ qua hoa thường, `-v` đảo, `-l` chỉ tên file, `-c` đếm, `-w` khớp nguyên từ, `-E` regex mở rộng.

### Drill 07

Dùng `find`: tìm mọi file `.cpp`; tìm file lớn hơn 1MB trong `/var/log`; tìm file sửa trong 1 ngày qua; xoá tất cả file trong `build/` có đuôi `.o`.

```sh
find . -name "*.cpp"
sudo find /var/log -type f -size +1M
find . -type f -mtime -1
find build -name "*.o" -delete          # hoặc: -exec rm {} \;
find . -name "*.c" -exec wc -l {} +     # đếm dòng mọi file .c
```

### Drill 08

Dùng `awk` in cột 1 (tên user) của `/etc/passwd`. In tên user và shell (cột 7) của những user có UID ≥ 1000.

```sh
awk -F: '{print $1}' /etc/passwd
awk -F: '$3 >= 1000 {print $1, $7}' /etc/passwd
ps aux | awk '{print $2, $11}'      # PID và lệnh
```
`-F:` đặt dấu phân cách; `$0` là cả dòng, `NR` số thứ tự dòng, `NF` số cột.

### Drill 09

Đếm mỗi loại shell (cột 7 của `/etc/passwd`) được bao nhiêu user dùng, sắp xếp giảm dần.

```sh
cut -d: -f7 /etc/passwd | sort | uniq -c | sort -nr
#     20 /usr/sbin/nologin
#      3 /bin/bash
#      1 /bin/sync
```
`uniq` chỉ gộp các dòng **liền kề** → phải `sort` trước. `sort -n` số học, `-r` đảo, `-k2` theo cột 2, `-u` duy nhất.

### Drill 10

Đếm số dòng, số từ, số byte của `/etc/passwd`. Đếm số file `.c` trong `bosch-lab`.

```sh
wc /etc/passwd          # dòng  từ  byte  tên
wc -l /etc/passwd
find . -name "*.c" | wc -l
ls | wc -l
```

### Drill 11

In 5 dòng đầu, 5 dòng cuối, và từ dòng 2 trở đi của `/etc/passwd`. Theo dõi log realtime.

```sh
head -n 5 /etc/passwd
tail -n 5 /etc/passwd
tail -n +2 /etc/passwd          # bỏ dòng đầu (header)
sed -n '3,6p' /etc/passwd       # dòng 3 tới 6
sudo tail -f /var/log/syslog    # Ctrl+C để thoát
```

### Drill 12

Tìm tiến trình `bash` đang chạy bằng `ps aux | grep` và bằng `pgrep`. Ý nghĩa các cột `VSZ`, `RSS`, `STAT` của `ps aux`?

```sh
ps aux | grep bash              # dòng "grep bash" cũng xuất hiện!
ps aux | grep [b]ash            # mẹo để loại chính lệnh grep
pgrep -a bash
ps -ef --forest                 # xem cây tiến trình (UID PID PPID ...)
ps -o pid,ppid,stat,ni,cmd -p $$
```
VSZ: bộ nhớ ảo (KB). RSS: bộ nhớ thật đang nằm trong RAM. STAT: R/S/D/T/Z, `+` foreground, `s` session leader, `<` ưu tiên cao, `N` ưu tiên thấp.

### Drill 13

Mở `top`: sắp xếp theo RAM, theo CPU, xem từng nhân, thoát. `load average` 3 con số nghĩa là gì?

```sh
top            # M: theo RAM, P: theo CPU, 1: từng nhân, k: kill, q: thoát
htop           # bản đẹp hơn (sudo apt install htop)
uptime         # load average: 0.52, 0.40, 0.35
nproc          # số nhân để so sánh với load
```
Load average = số tiến trình trung bình đang chạy hoặc chờ CPU (và chờ I/O trạng thái D) trong 1, 5, 15 phút.

### Drill 14

Chạy `sleep 100`, nhấn `Ctrl+Z`, đưa nó chạy nền, liệt kê job, đưa về foreground, rồi kết thúc nó. Chạy thẳng một lệnh ở chế độ nền.

```sh
sleep 100
^Z                  # [1]+  Stopped   sleep 100   (gửi SIGTSTP)
bg %1               # chạy tiếp ở nền (gửi SIGCONT)
jobs -l             # [1]+ 12345 Running   sleep 100 &
fg %1               # về foreground
^C                  # SIGINT
sleep 200 &         # chạy nền ngay; $! là PID của nó
kill %1             # kill theo số job
nohup ./server &    # vẫn chạy sau khi đóng terminal (bỏ qua SIGHUP)
```

### Drill 15

Liệt kê tên các signal. Khác nhau giữa `kill PID`, `kill -9 PID`, `pkill`, `killall`?

```sh
kill -l                 # danh sách signal
kill 1234               # gửi SIGTERM (15) - có thể bị bắt/bỏ qua
kill -9 1234            # SIGKILL - không bắt được
kill -SIGSTOP 1234; kill -CONT 1234
pkill -f "python app"   # theo pattern (-f khớp cả dòng lệnh)
killall sleep           # theo tên chính xác
```

### Drill 16

So sánh `df -h` và `du -sh`. Thư mục nào trong `/var` chiếm nhiều chỗ nhất?

```sh
df -h                   # dung lượng còn trống của từng FILESYSTEM (đã mount)
du -sh ~/bosch-lab      # tổng dung lượng một THƯ MỤC
sudo du -sh /var/* 2>/dev/null | sort -h | tail -3
df -i                   # số inode còn trống
lsblk                   # các ổ đĩa / phân vùng
```
Hai số có thể lệch: file đã xoá nhưng còn tiến trình mở thì `df` vẫn tính, `du` thì không.

### Drill 17

Chạy `make run T=03-process/05_zombie` ở terminal 1. Ở terminal 2, xem các file mà tiến trình đó đang mở bằng `lsof` và `/proc`. Xem tiến trình nào đang nghe cổng 22.

```sh
pgrep 05_zombie
lsof -p <PID>               # cwd, txt (chương trình), mem (thư viện .so), 0u 1u 2u (/dev/pts/N)
ls -l /proc/<PID>/fd        # 0 -> /dev/pts/1, 1 -> /dev/pts/1, 2 -> /dev/pts/1
cat /proc/<PID>/status | head
sudo lsof -i :22            # tiến trình nghe cổng 22
ss -tlnp                    # các cổng TCP đang listen
```

### Drill 18

Dùng `strace` xem các system call của `01_fork_count`: thống kê số lần gọi, và chỉ xem `clone`/`write`. Dùng `ltrace` cho lời gọi thư viện.

```sh
strace -c ./build/03-process/01_fork_count          # bảng thống kê syscall
strace -f -e trace=clone,write ./build/03-process/01_fork_count
strace -p <PID>                                     # gắn vào tiến trình đang chạy
ltrace ./build/01-c/07_signed_unsigned              # printf, strlen...
```
glibc hiện thực `fork()` bằng system call `clone` (thấy `clone(child_stack=NULL, flags=...SIGCHLD...)`). `-f` theo dõi cả tiến trình con.

### Drill 19

In mã thoát của lệnh trước, PID của shell hiện tại, PID của job nền gần nhất. Thêm `~/bin` vào PATH. Khác nhau giữa biến shell và biến môi trường?

```sh
ls /khong_co; echo $?       # 2 (khác 0 = lỗi)
true; echo $?               # 0
echo $$                     # PID của shell
sleep 50 & echo $!          # PID job nền vừa chạy
export PATH="$HOME/bin:$PATH"
X=1; bash -c 'echo "X=$X"'          # X=   (biến shell không truyền cho tiến trình con)
export X; bash -c 'echo "X=$X"'     # X=1
cmd1 && cmd2    # cmd2 chạy khi cmd1 thành công;   cmd1 || cmd2  khi thất bại
```
Trong script: `$0` tên script, `$1..$9` đối số, `$#` số đối số, `$@` tất cả đối số.

### Drill 20

Quan sát 4 giai đoạn biên dịch của `01-c/08_macro_traps.c`. Xem thư viện động mà file thực thi dùng, và các symbol trong file `.o`.

```sh
gcc -E 01-c/08_macro_traps.c | tail -20    # 1. tiền xử lý: thấy macro đã bị thay
gcc -S 01-c/08_macro_traps.c -o m.s        # 2. compile ra assembly
gcc -c 01-c/08_macro_traps.c -o m.o        # 3. assemble ra object
gcc m.o -o m                               # 4. link
file m                 # ELF 64-bit LSB pie executable, x86-64, dynamically linked
ldd m                  # libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6
nm m.o                 # T main  (T: trong text), U printf (U: chưa định nghĩa)
objdump -d m.o | head
```
Thư viện tĩnh `.a` (copy vào file chạy lúc link, `ar rcs`), thư viện động `.so` (nạp lúc chạy, cần `-fPIC -shared`, tìm qua `LD_LIBRARY_PATH`/`ldconfig`).


## 10-alp

Các bài theo sách *Advanced Linux Programming* (Mitchell, Oldham, Samuel, 2001), chương 1–5. Tên hàm và ví dụ bám theo listing trong sách.

### 01_getopt_long — getopt_long: tùy chọn ngắn/dài, optarg, optind, "--" và hoán vị argv

**Câu hỏi:** Với mỗi argv mẫu, getopt_long trả về những tùy chọn nào, optarg là gì, optind dừng ở đâu? "-vo x.txt" được hiểu thế nào? Sau "--" thì "-h" còn là tùy chọn không? Gặp "-x" (không khai báo) và "-o" thiếu đối số thì sao?

**Output mong đợi:**

```text
Mau 1: -v -o out.txt in1 in2
  -v/--verbose
  -o/--output, optarg = out.txt
  optind = 4, doi so con lai: in1 in2
Mau 2: --output=log.txt --verbose file
  -o/--output, optarg = log.txt
  -v/--verbose
  optind = 3, doi so con lai: file
Mau 3: -vo x.txt -- -h
  -v/--verbose
  -o/--output, optarg = x.txt
  optind = 4, doi so con lai: -h
Mau 4: in1 -v in2 --output o.txt
  -v/--verbose
  -o/--output, optarg = o.txt
  optind = 4, doi so con lai: in1 in2
  argv sau khi hoan vi: -v --output o.txt in1 in2
Mau 5: -x -o
prog: invalid option -- 'x'
  '?' (optopt = 'x')
prog: option requires an argument -- 'o'
  '?' (optopt = 'o')
  optind = 3, doi so con lai:
```

**Giải thích:**

- `"ho:v"`: `o` có dấu `:` nên cần đối số; đối số có thể là từ tiếp theo (`-o out.txt`) hoặc dính liền (`-oout.txt`). Tùy chọn dài: `--output=log.txt` hoặc `--output log.txt`.
- `-vo x.txt`: gộp nhiều tùy chọn ngắn; `v` không cần đối số, `o` lấy từ tiếp theo.
- `--` kết thúc phần tùy chọn: mọi thứ phía sau (kể cả `-h`) là đối số thường.
- GNU getopt **hoán vị** argv: các đối số thường (`in1`, `in2`) bị dồn xuống cuối, nên sau vòng lặp chỉ cần duyệt từ `optind` tới `argc`.
- Tùy chọn không khai báo hoặc thiếu đối số → trả về `'?'`, ký tự gây lỗi nằm trong `optopt`, thông báo lỗi in ra **stderr** (tắt bằng `opterr = 0`). Listing 2.2 khi đó gọi `print_usage(stderr, 1)`.

**Liên hệ đề thi:** Quy ước GNU: tùy chọn ngắn `-h`, tùy chọn dài `--help`; mọi chương trình nên có `--help` và `--version`. Mảng `struct option` phải kết thúc bằng phần tử `{NULL, 0, NULL, 0}`. Trường `has_arg` = 0 (không có), 1 (bắt buộc), 2 (tùy chọn).

### 02_env_exit_code — Biến môi trường và exit code, kể cả khi bị signal giết

**Câu hỏi:** setenv(..., 0) có ghi đè không? Process con và chương trình exec sau đó có thấy biến vừa setenv không? system("exit 300") cho WEXITSTATUS bằng mấy? Shell bị SIGTERM thì wait status là gì, và $? mà shell cha thấy là bao nhiêu?

**Output mong đợi:**

```text
environ co > 0 bien, PATH co
SERVER_NAME = (NULL) -> dung mac dinh server.my-company.com
sau setenv: SERVER_NAME = backup.example.net
process con thay: backup.example.net
chuong trinh sau exec thay: backup.example.net
sau unsetenv: NULL
system("exit 3"): WIFEXITED, WEXITSTATUS = 3
system("exit 300"): WIFEXITED, WEXITSTATUS = 44
system("kill -TERM $$"): WIFSIGNALED, WTERMSIG = 15 (Terminated)
Terminated
  shell thay $? = 143
system("sh -c 'kill -TERM $$'; echo "  shell thay \$? = $?"; exit 0"): WIFEXITED, WEXITSTATUS = 0
system("khong_co_lenh_nay 2>/dev/null"): WIFEXITED, WEXITSTATUS = 127
```

**Giải thích:**

- `setenv(name, value, 0)`: không ghi đè nếu biến đã có.
- Môi trường được copy sang process con khi fork và giữ nguyên qua exec (exec không có hậu tố `e`).
- `getenv` trả về NULL khi không có biến → chương trình dùng giá trị mặc định (listing 2.4).
- Exit code chỉ giữ 8 bit thấp: 300 mod 256 = 44. Vì vậy sách khuyên chỉ dùng 0–127.
- Shell bị SIGTERM: wait status là "bị signal 15", không có exit code. Khi đó shell cha báo `$?` = 128 + 15 = 143 và in "Terminated".
- Không tìm thấy lệnh: shell trả về 127 (126 nếu có file nhưng không chạy được).

**Liên hệ đề thi:** `printenv`, `export VAR=value`, `echo $?`. Biến `environ` (`extern char **environ;`) là mảng `"TEN=giatri"` kết thúc bằng NULL; không sửa trực tiếp mà dùng `setenv`/`unsetenv`. Process con không bao giờ đổi được môi trường của process cha.

### 03_stdio_buffer — Buffer của stdio: full, line, không buffer; exit và _exit

**Câu hỏi:** Với stdout đặt full buffering, các dòng [stdout] và [stderr] hiện theo thứ tự nào? Ở phần hai, chữ A/B/C/D/E hiện theo thứ tự nào? Dòng cuối có hiện ra không?

**Output mong đợi:**

```text
[stderr 2] [stderr 4]
[stdout 1] [stdout 3]
C(none) B(line) D(line)
E(none)
A(full)
```

**Giải thích:**

- stdout full-buffered: `[stdout 1]` và `[stdout 3]` nằm trong buffer tới khi `fflush`. stderr không buffer nên ra trước.
- Phần hai: `C` (không buffer) ra ngay; `B` nằm trong buffer line-buffered tới khi gặp `'\n'` của `D`; `E` ra ngay; `A` (full) chỉ ra khi `fclose`.
- Dòng cuối nằm trong buffer của stdout; `_exit` không flush stdio nên dòng đó mất. `exit()` (hoặc return từ main) thì có flush.

**Liên hệ đề thi:** stdout ra terminal là line-buffered, ra file/pipe là full-buffered (vì vậy `./app | cat` có thể đổi thứ tự so với stderr). Ví dụ trong sách: `while (1) { printf("."); sleep(1); }` không in mỗi giây một dấu chấm; đổi sang stderr thì in. Gộp hai luồng: `prog > out.txt 2>&1` (2>&1 đứng **sau** redirect file) và `prog 2>&1 | filter` (đứng **trước** pipe).

### 04_tempfile — File tạm an toàn: mkstemp + unlink ngay, tmpfile

**Câu hỏi:** mkstemp tạo file với quyền gì? Sau unlink, access() thấy file không, còn read qua fd thì sao? st_nlink trước và sau unlink?

**Output mong đợi:**

```text
ten file: /tmp/alp_temp.<6 ky tu>, con XXXXXX ? khong, do dai = 20
quyen = 600, st_nlink = 1
sau unlink: access() = -1 (No such file or directory), st_nlink = 0
doc lai qua fd: length = 12, data = "du lieu tam"
tmpfile: doc lai 42 tmpfile, fileno(f) > 2 ? 1
```

**Giải thích:**

- Mẫu tên phải kết thúc bằng đúng 6 chữ `X`; mkstemp sửa thẳng vào mảng (nên phải là mảng `char[]`, không phải string literal).
- Quyền 0600: chỉ chủ sở hữu đọc/ghi được.
- `unlink` xoá tên (link count về 0) nhưng inode còn sống vì fd vẫn mở → vẫn đọc ghi được. Khi đóng fd hoặc process chết thì file mới thật sự bị xoá, nên không để lại rác trong /tmp.
- Listing 2.5 ghi độ dài trước rồi mới ghi dữ liệu, `lseek` về đầu rồi đọc lại.
- `tmpfile()` trả về `FILE*` đã được unlink sẵn.

**Liên hệ đề thi:** Ba yêu cầu với file tạm: tên không trùng giữa các instance, quyền không cho người khác sửa, tên không đoán trước được (tránh race giữa lúc kiểm tra và lúc mở). Dùng mkstemp (fd, có thể đưa tên cho chương trình khác) hoặc tmpfile (FILE*, chỉ dùng nội bộ). Không dùng mktemp, tmpnam, tempnam.

### 05_errno_assert — errno, strerror, perror và assert ⚠️ cố ý chết vì SIGABRT

**Câu hỏi:** errno của từng lời gọi lỗi là gì? Sau một lời gọi THÀNH CÔNG, errno có về 0 không? Với NDEBUG, assert(do_something() == 0) gọi hàm mấy lần? Dòng cuối kết thúc chương trình thế nào?

**Output mong đợi:**

```text
open("/khong/ton/tai")       -> -1, errno =  2 ENOENT  (No such file or directory)
getpid() (thanh cong)        ->  1, errno =  2 ENOENT  (No such file or directory)
open("/tmp", O_WRONLY)       -> -1, errno = 21 EISDIR  (Is a directory)
close(12345)                 -> -1, errno =  9 EBADF   (Bad file descriptor)
mkdir("/tmp")                -> -1, errno = 17 EEXIST  (File exists)
lseek(0, 0, 12345)           -> -1, errno = 22 EINVAL  (Invalid argument)
open("/etc/shadow")          -> -1, errno = 13 EACCES  (Permission denied)
perror in ra stderr: No such file or directory
NDEBUG: calls = 0
khong NDEBUG: calls = 3
05_errno_assert: 10-alp/05_errno_assert.c:64: main: Assertion `pointer != NULL' failed.
(shell báo Aborted (core dumped), exit code 134 = 128 + 6)
```

**Giải thích:**

- System call lỗi trả về -1 và đặt errno. Lời gọi thành công **không** đặt lại errno → getpid xong errno vẫn là ENOENT từ lần trước.
- Phải lưu errno ngay (`int saved = errno;`), vì printf hay hàm khác có thể đổi nó.
- `perror("msg")` in `msg: <strerror(errno)>` ra stderr.
- Với NDEBUG, assert bị preprocessor xoá → `do_something()` không được gọi lần nào. Đó là lý do biểu thức trong assert không được có tác dụng phụ (sách: gán kết quả vào biến trước rồi `assert(status == 0)`).
- assert sai: in tên chương trình, file, dòng, hàm, biểu thức, rồi `abort()` → SIGABRT.

**Liên hệ đề thi:** Mã lỗi hay gặp: EPERM, ENOENT, EINTR (bị signal ngắt → gọi lại), EBADF, EACCES, EEXIST, ENOTDIR, EISDIR, EINVAL, ENOMEM, EAGAIN. Dùng assert cho kiểm tra **nội bộ** (con trỏ NULL, điều kiện tham số), không dùng để kiểm tra input của người dùng.

### 06_dlopen — Nạp thư viện động lúc chạy: dlopen, dlsym, dlerror, dlclose

**Câu hỏi:** Gọi cos qua con trỏ lấy từ dlsym được không? dlsym một tên không có, hoặc dlopen một thư viện không có thì nhận gì? dlerror() gọi hai lần liên tiếp trả về gì?

**Output mong đợi:**

```text
cos(0) = 1.0, sqrt(2) = 1.4142
dlsym ten khong co: (nil)
dlerror() lan 1: /lib/x86_64-linux-gnu/libm.so.6: undefined symbol: khong_co_ham_nay
dlerror() lan 2: (NULL)
dlopen thu vien khong co: (nil), dlerror: libkhongco.so: cannot open shared object file: No such file or directory
dlclose = 0
```

**Giải thích:**

- dlsym trả về `void*`, ép sang kiểu con trỏ hàm phù hợp rồi gọi.
- Lỗi được báo bằng NULL; chi tiết lấy qua `dlerror()`. Sau khi đọc, lỗi bị xoá nên lần gọi thứ hai trả về NULL.
- dlopen tìm thư viện theo LD_LIBRARY_PATH, cache của ldconfig, /lib, /usr/lib.

**Liên hệ đề thi:** Dùng cho plugin. `RTLD_LAZY` (giải quyết symbol khi gọi lần đầu) và `RTLD_NOW` (giải quyết hết ngay). Hàm trong thư viện C++ phải `extern "C"`. Link bằng `-ldl` (glibc 2.34 trở đi đã gộp vào libc). dlopen cùng một thư viện nhiều lần chỉ tăng bộ đếm tham chiếu.

### 07_fork_exec — system, fork + exec, spawn, nice

**Câu hỏi:** Dòng sau exec có in ra không? execvp một lệnh không tồn tại thì process con làm gì, cha thấy status gì? Biến HOME có trong chương trình chạy bằng execle không? nice(10) trong con làm priority của cha đổi không?

**Output mong đợi:**

```text
1) system:
  [sh] $0 = sh
  system tra ve 0
2) spawn("echo", ...) bang fork + execvp:
  [echo] xin chao tu chuong trinh moi
  [cha] con thoat binh thuong, exit code 0
3) spawn lenh khong ton tai:
  [con] execvp loi: No such file or directory
  [cha] con bi signal 6 (Aborted)
4) execl (duong dan day du) va dong sau exec:
  [echo] chay bang execl
  [cha] con thoat binh thuong, exit code 0
5) execle voi moi truong tu chon:
  [sh] GREETING=xin chao, HOME=(khong co)
  [cha] con thoat binh thuong, exit code 0
6) nice:
  [con] nice: 0 -> 10
  [cha] nice van la 0
```

**Giải thích:**

- `system()` chạy lệnh qua `/bin/sh -c` → `$0` là `sh`.
- exec thành công thì không bao giờ quay về; chỉ khi exec lỗi thì code phía sau mới chạy. Trong listing 3.4, con gọi `abort()` sau exec lỗi → cha thấy SIGABRT.
- execle dùng đúng mảng môi trường được truyền vào: chỉ có GREETING, không có HOME.
- nice là thuộc tính của từng process; con tăng niceness (giảm ưu tiên) không ảnh hưởng cha. Niceness từ -20 (ưu tiên cao nhất) tới 19; chỉ root mới giảm được.

**Liên hệ đề thi:** Họ exec: `execl`, `execlp`, `execle`, `execv`, `execvp`, `execve` (system call thật). `p`: tìm trong PATH; `v`: argv là mảng; `l`: đối số liệt kê và kết thúc bằng `(char *)NULL`; `e`: truyền môi trường. Sách khuyên dùng fork + exec thay vì system vì system phụ thuộc shell, có lỗ hổng bảo mật và chậm.

### 08_wait_status — wait/waitpid, các macro trạng thái, WNOHANG, zombie

**Câu hỏi:** exit(300) cho WEXITSTATUS bao nhiêu? abort() và kill -9 thì WIFSIGNALED/WTERMSIG là gì? waitpid(..., WNOHANG) trả về gì khi con chưa xong? Con đã thoát nhưng chưa được wait thì trạng thái trong /proc là gì? wait() khi không còn con?

**Output mong đợi:**

```text
con exit(300):         WIFEXITED   WEXITSTATUS = 44
con exit(-1):          WIFEXITED   WEXITSTATUS = 255
con abort():           WIFSIGNALED WTERMSIG = 6 (Aborted)
con bi kill -9:        WIFSIGNALED WTERMSIG = 9 (Killed)
WNOHANG khi con chua xong: waitpid = 0
con da thoat, chua wait: trang thai trong /proc = Z
WNOHANG lan 2: tra ve dung PID con ? 1
con exit(7):           WIFEXITED   WEXITSTATUS = 7
sau khi wait: /proc/<pid> = - (khong con)
wait() khi khong con con: -1, errno = ECHILD
```

**Giải thích:**

- Chỉ giữ 8 bit thấp: 300 → 44, -1 → 255.
- Bị signal giết thì không có exit code; dùng `WTERMSIG` để biết signal nào.
- `WNOHANG`: trả về 0 nếu chưa có con nào kết thúc, không chặn (dùng để kiểm tra định kỳ).
- Con đã thoát mà cha chưa wait → **zombie** (`Z` trong ps/proc): chỉ còn giữ PID và trạng thái thoát. Sau khi wait thì entry biến mất hoàn toàn.
- Không còn con nào → wait trả về -1, errno = ECHILD.

**Liên hệ đề thi:** `wait` chặn tới khi có một con bất kỳ kết thúc; `waitpid(pid, &status, options)` chờ một con cụ thể (pid = -1 là bất kỳ); `wait3`/`wait4` trả thêm thông tin tài nguyên đã dùng. Cha chết trước thì con zombie được init nhận nuôi và dọn. Cách dọn tự động: handler SIGCHLD gọi wait (listing 3.7), hoặc gán SIGCHLD = SIG_IGN.

### 09_thread_return — pthread: truyền dữ liệu, giá trị trả về, pthread_exit, detached ⚠️ không cố định

**Câu hỏi:** Số nguyên tố thứ 1000 mà thread tính được là bao nhiêu? pthread_equal(self, main) trong thread con? Gọi pthread_join lên thread detached thì sao? Hai thread in ký tự dùng chung struct trên stack của main có an toàn không?

**Output mong đợi:**

```text
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxoooooooooooooooooooo     <- 30 x và 20 o, thứ tự xen kẽ có thể khác
(30 x va 20 o, thu tu xen ke khong co dinh)
so nguyen to thu 1000 = 7919
trong thread con: pthread_equal(self, main) = 0
pthread_exit((void*)7) -> join nhan 7
pthread_join thread detached: 22 (EINVAL)
```

**Giải thích:**

- Listing 4.2/4.3: mỗi thread nhận struct riêng qua `void*`. An toàn vì main join cả hai trước khi struct (trên stack của main) hết hạn. Nếu main return trước thì thread đọc vùng nhớ đã bị huỷ (lỗi sách nêu ở listing 4.2).
- Listing 4.4: giá trị trả về ép thành `void*` và lấy lại qua tham số thứ hai của `pthread_join`.
- `pthread_exit(v)` tương đương `return v` từ hàm thread.
- So sánh thread ID bằng `pthread_equal`, không dùng `==` (pthread_t có thể là struct).
- Thread detached không join được: glibc trả về EINVAL (POSIX coi đây là undefined behavior).

**Liên hệ đề thi:** Biên dịch với `-pthread` (sách dùng `-lpthread`). Hàm pthread trả về **mã lỗi** chứ không đặt errno. Thread joinable mà không ai join thì sẽ thành "zombie thread" cho tới khi được join; thread không cần kết quả thì nên detach (`pthread_detach` hoặc thuộc tính `PTHREAD_CREATE_DETACHED`).

### 10_thread_cancel — Huỷ thread: deferred, vùng không huỷ được, cleanup handler

**Câu hỏi:** Thread A bị huỷ trong lúc sleep: cleanup handler có chạy không, join nhận được gì? Thread B tắt cancel trong lúc "chuyển tiền": bị huỷ ở đâu, dòng "chuyen xong" có in ra không? pthread_cleanup_pop(1) khi không bị huỷ thì sao?

**Output mong đợi:**

```text
Thread A:
  [cleanup] free buffer
  join A: PTHREAD_CANCELED
Thread B:
  [B] bat dau chuyen tien (cancel bi tat)
  [main] pthread_cancel(B) = 0
  [B] chuyen xong
  join B: PTHREAD_CANCELED
Thread C:
  [C] chay xong binh thuong, pop(1) van goi handler:
  [cleanup] free buffer
  join C: 42
```

**Giải thích:**

- A là deferred cancelable (mặc định): bị huỷ tại `usleep` (cancellation point) → cleanup handler chạy, giải phóng buffer → join nhận `PTHREAD_CANCELED`.
- B tắt cancel: yêu cầu huỷ bị **giữ lại**; B làm xong việc, bật lại cancel rồi bị huỷ tại `pthread_testcancel()` → không in "khong bao gio toi day". Đây là ý của listing 4.6 (chuyển tiền giữa hai tài khoản không được dừng giữa chừng).
- `pthread_cleanup_pop(1)` gỡ handler và chạy nó; `pop(0)` chỉ gỡ mà không chạy. push/pop phải đi cặp trong cùng một khối (chúng là macro mở/đóng `{ }`).

**Liên hệ đề thi:** Ba trạng thái: asynchronously cancelable (huỷ bất cứ lúc nào, nguy hiểm), synchronously/deferred cancelable (mặc định, huỷ tại cancellation point), uncancelable. Hàm: `pthread_cancel`, `pthread_setcanceltype`, `pthread_setcancelstate`, `pthread_testcancel`. Trong C++, sách khuyên ném exception để thoát thread, nhờ đó destructor của object local được gọi.

### 11_tsd_mutex — Thread-specific data và các loại mutex, semaphore

**Câu hỏi:** Mỗi thread đọc pthread_getspecific được giá trị của ai? Destructor của key chạy khi nào? Lock lần hai mutex error-checking trả về gì, mutex recursive thì sao? Thread khác unlock mutex error-checking? trylock khi mutex đang bị giữ? sem_trywait khi giá trị là 0?

**Output mong đợi:**

```text
Thread-specific data:
  thread1.log -> ghi "Thread starting."
  [destructor] dong "thread1.log"
  thread2.log -> ghi "Thread starting."
  [destructor] dong "thread2.log"
  thread3.log -> ghi "Thread starting."
  [destructor] dong "thread3.log"
  main: pthread_getspecific = NULL
Mutex:
  errorcheck lock lan 1: 0
  errorcheck lock lan 2: EDEADLK
  thread khac unlock errorcheck: EPERM
  errorcheck unlock (chu so huu): 0
  recursive lock, lock: 0 0; unlock x3: 0 0 EPERM
  fast: trylock trong chinh thread dang giu: EBUSY
  thread khac trylock mutex dang bi giu: EBUSY
Semaphore:
  khoi tao 2, sem_getvalue = 2
  sau 2 lan sem_wait: sem_trywait = -1, errno = EAGAIN
  sau sem_post: 1
```

**Giải thích:**

- Listing 4.7: một key `thread_log_key`, mỗi thread gắn tên file log riêng. Hàm `write_to_thread_log` không cần tham số mà vẫn ghi đúng file của thread đang gọi. Main chưa set gì nên nhận NULL.
- Destructor của key tự chạy khi thread kết thúc (với giá trị khác NULL), giống listing 4.7 đóng file log.
- Error-checking: lock lại trong cùng thread → EDEADLK thay vì treo; unlock bởi thread không sở hữu → EPERM.
- Recursive: lock 2 lần thì phải unlock 2 lần; unlock lần thứ 3 → EPERM.
- trylock không bao giờ chặn: đang bị giữ (kể cả bởi chính mình) → EBUSY.
- Semaphore đếm: hai lần wait đưa về 0; `sem_trywait` không chặn mà trả về -1, errno = EAGAIN (semaphore dùng errno, khác với mutex).

**Liên hệ đề thi:** Ba loại mutex trong sách: fast (mặc định, lock lại thì deadlock), recursive, error-checking. `PTHREAD_MUTEX_INITIALIZER` khởi tạo tĩnh. Semaphore cho thread: `sem_init(&s, 0, giá trị)` (tham số 0 = chỉ dùng giữa các thread của một process), `sem_wait`, `sem_post`, `sem_trywait`, `sem_getvalue`, `sem_destroy`. Hàng đợi công việc (job queue) trong sách dùng mutex + semaphore đếm số job.

### 12_sysv_shm_sem — System V shared memory và semaphore

**Câu hỏi:** shm_nattch bằng mấy trước fork, trong lúc con còn sống, sau khi con thoát? Cha đọc được chuỗi con ghi vào vùng nhớ chung không? semop -1 với IPC_NOWAIT khi giá trị là 0? Con tăng semaphore với SEM_UNDO rồi thoát thì giá trị còn lại bao nhiêu?

**Output mong đợi:**

```text
segment: size = 4096 (= getpagesize()), nattch = 1
semop -1 IPC_NOWAIT khi = 0: -1, errno = EAGAIN
  [con] da ghi, nattch = 2
  [cha] doc duoc: "Hello, world (tu con)"
sau khi con thoat: nattch = 1
  [con] +3 voi SEM_UNDO, gia tri = 3
con thoat, kernel hoan tac: gia tri = 0
sau shmdt cua cha: nattch = 0
sau IPC_RMID: shmctl IPC_STAT -> EINVAL/EIDRM (da xoa)
```

**Giải thích:**

- Kích thước segment nên là bội số của page size (`getpagesize()`, thường 4096).
- fork làm con kế thừa vùng đã attach → nattch = 2; process thoát thì tự detach.
- Con ghi, cha đọc được ngay: cùng vùng nhớ vật lý, không có copy. Semaphore đảm bảo cha chỉ đọc sau khi con đã ghi.
- `IPC_NOWAIT`: semop không chặn mà trả về EAGAIN.
- `SEM_UNDO`: kernel ghi nhớ thay đổi và hoàn tác khi process kết thúc. Nhờ đó semaphore không bị kẹt nếu process chết giữa chừng.
- Segment chỉ biến mất sau `IPC_RMID` (và khi nattch về 0).

**Liên hệ đề thi:** Lệnh: `ipcs -m` (shm), `ipcs -s` (semaphore), `ipcrm`. Shared memory là IPC **nhanh nhất** vì không qua kernel khi đọc/ghi, nhưng phải tự đồng bộ. `shmget(key, size, IPC_CREAT | IPC_EXCL | quyền)`, `shmat(id, NULL, 0)`, `shmdt(addr)`, `shmctl(id, IPC_RMID, NULL)`. semop dùng mảng `struct sembuf {sem_num, sem_op, sem_flg}`; semctl cần tự khai báo `union semun`.

### 13_mmap_shared_private — mmap: MAP_SHARED, MAP_PRIVATE, msync, chia sẻ qua fork

**Câu hỏi:** Ghi vào vùng MAP_PRIVATE có làm đổi file không? Vùng MAP_SHARED thấy gì? Sau khi ghi qua MAP_SHARED, đọc file bằng pread thấy gì? Con tăng counter trong vùng MAP_SHARED|MAP_ANONYMOUS, cha thấy bao nhiêu, còn với MAP_PRIVATE?

**Output mong đợi:**

```text
ban dau: shared = "ban goc", private = "ban goc"
ghi qua MAP_PRIVATE: private = "sua qua PRIVATE", shared = "ban goc", file = "ban goc"
msync = 0
ghi qua MAP_SHARED: shared = "sua qua SHARED", file = "sua qua SHARED", private = "sua qua PRIVATE"
sau khi con tang 1000 lan: MAP_SHARED = 1000, MAP_PRIVATE = 0
```

**Giải thích:**

- MAP_PRIVATE là copy-on-write: lần ghi đầu tạo bản copy riêng của trang, file và các mapping khác không thấy.
- MAP_SHARED ghi thẳng vào page cache của file → `pread` thấy ngay. `msync(MS_SYNC)` chờ ghi xuống đĩa.
- Trang private đã được copy nên không còn thấy thay đổi của file.
- Anonymous shared mapping được kế thừa qua fork và dùng chung → cha thấy 1000. Private mapping thì fork copy-on-write → cha vẫn 0.

**Liên hệ đề thi:** `mmap(addr, length, prot, flags, fd, offset)`; prot: `PROT_READ`/`PROT_WRITE`/`PROT_EXEC`; flags: `MAP_SHARED`/`MAP_PRIVATE` (chọn một), `MAP_FIXED`, `MAP_ANONYMOUS`. Lỗi trả về `MAP_FAILED`, không phải NULL. File phải đủ dài trước khi map (sách dùng lseek + write một byte ở cuối). Truy cập vượt cuối file → SIGBUS. Các flag của msync: `MS_ASYNC`, `MS_SYNC`, `MS_INVALIDATE`.

### 14_popen_fdopen — Pipe với FILE*: fdopen, dup2 + exec sort, popen/pclose

**Câu hỏi:** Các dòng gửi cho sort hiện ra lúc nào và theo thứ tự nào? popen("...; exit 3", "r") thì pclose trả về status gì? Process con đọc pipe bằng fgets dừng khi nào?

**Output mong đợi:**

```text
1) pipe + fdopen:
  [con] nhan: Hello, world #1
  [con] nhan: Hello, world #2
  [con] nhan: Hello, world #3
  [con] fgets tra ve NULL: EOF vi cha da dong dau ghi
2) dup2 + execlp("sort"):
  [cha] da ghi xong 5 dong, sort chua in gi vi chua thay EOF
  Hello, world.
  My dog has fleas.
  One fish, two fish.
  This is a test.
  This program is great.
3) popen("sort", "w"):
  cam
  chuoi
  tao
  pclose = 0 (WEXITSTATUS 0)
4) popen(..., "r"):
  doc duoc "42", pclose -> WEXITSTATUS 3
```

**Giải thích:**

- fgets trả về NULL khi mọi đầu ghi đã đóng (con đã đóng `fds[1]` của mình, cha thì fclose).
- sort phải đọc hết input (tới EOF) mới in được → chỉ in sau khi cha `close(fds[1])`.
- popen "w": ta ghi vào stdin của lệnh; `pclose` đóng pipe (gửi EOF), chờ con và trả về wait status.
- popen "r": đọc stdout của lệnh; exit code của lệnh lấy từ `WEXITSTATUS(pclose(...))`.

**Liên hệ đề thi:** Pipe một chiều, dữ liệu theo thứ tự FIFO, chỉ dùng giữa các process có quan hệ họ hàng (FIFO có tên `mkfifo` thì không cần). Ghi vào pipe không còn đầu đọc → SIGPIPE/EPIPE. Đọc pipe rỗng khi đầu ghi vẫn mở → chặn. popen có cùng rủi ro bảo mật với system vì chạy qua shell.

### 15_sockets — socketpair, socket Internet qua 127.0.0.1, byte order

**Câu hỏi:** socketpair cho giao tiếp một chiều hay hai chiều? htons(80) trên máy x86 bằng bao nhiêu (hex)? Server bind port 0 thì được port nào? read trả về 0 khi nào?

**Output mong đợi:**

```text
htons(80) = 0x5000, htonl(0x7f000001) = 0x0100007f, ntohs(htons(8080)) = 8080
  [con] nhan "ping", tra loi pong
  [cha] nhan "pong" qua cung socket (hai chieu)
  [cha] read sau khi con dong socket = 0 (EOF)
server lang nghe tai 127.0.0.1, port > 1023 ? 1
  [server] accept tra ve socket moi (khac socket lang nghe ? 1), nhan: GET /
  [client] nhan: HTTP/1.0 200 OK
```

**Giải thích:**

- x86 là little-endian, network byte order là big-endian → htons đảo hai byte: 80 = 0x0050 → 0x5000.
- socketpair là hai chiều: cùng một fd vừa ghi vừa đọc (khác pipe).
- Bên kia đóng kết nối → read trả về 0 (EOF).
- Port 0 → kernel cấp một port tạm (ephemeral) còn trống; đọc lại bằng `getsockname`.
- `accept` trả về một socket **mới** cho kết nối này; socket gốc tiếp tục lắng nghe client khác.

**Liên hệ đề thi:** Ba tham số của socket: namespace (`PF_LOCAL`/`PF_UNIX`, `PF_INET`), style (`SOCK_STREAM`: kết nối, đúng thứ tự, tin cậy; `SOCK_DGRAM`: gói tin, có thể mất/đảo thứ tự), protocol (0 = mặc định). Local socket dùng đường dẫn file (`sockaddr_un.sun_path`) và cần `unlink` sau khi dùng. Port dưới 1024 chỉ root mới bind được. Sách dùng `gethostbyname` để phân giải tên (nay nên dùng `getaddrinfo`).
