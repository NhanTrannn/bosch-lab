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

