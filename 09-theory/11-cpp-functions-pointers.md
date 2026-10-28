# Hàm, con trỏ và tham chiếu

## Lý thuyết

Theo Udemy *Beginning C++ Programming*, section 11–12. Lab: `02-cpp/21_functions`, `02-cpp/22_pointers_refs`, xem thêm `01-c/02`, `01-c/03`, `01-c/13`.

### Hàm (section 11)
- **Prototype** (khai báo) cho compiler biết tên, kiểu trả về, kiểu tham số. Định nghĩa phải xuất hiện đúng một lần trong cả chương trình (ODR). Prototype thường đặt trong `.h`, định nghĩa trong `.cpp`.
- Tham số (parameter) là biến trong định nghĩa; đối số (argument) là giá trị truyền vào khi gọi.
- **Pass by value** (mặc định): hàm nhận bản copy, sửa không ảnh hưởng biến gốc; copy object lớn tốn kém.
- **Pass by reference** `T&`: hàm làm việc trên biến gốc, không copy. Dùng `const T&` khi chỉ đọc (vừa rẻ vừa an toàn, nhận được cả giá trị tạm).
- **Pass by pointer** `T*`: truyền địa chỉ; có thể là `nullptr`. Bản thân con trỏ vẫn được truyền theo giá trị.
- `return`: hàm `void` có thể không return. Không bao giờ trả về reference/con trỏ tới biến local (dangling).
- **Tham số mặc định**: chỉ ghi ở khai báo (prototype), không ghi lại ở định nghĩa; phải là các tham số **cuối cùng**: `void f(int a, int b = 1, int c = 2);`.
- **Overloading**: cùng tên, khác danh sách tham số (số lượng/kiểu/thứ tự). **Không** overload chỉ bằng kiểu trả về. Thứ tự chọn: khớp chính xác → promotion (`char/short → int`, `float → double`) → chuyển đổi chuẩn (`int → double`, `const char* → bool`) → chuyển đổi do người dùng định nghĩa (constructor, operator chuyển kiểu). Hai ứng viên cùng hạng → lỗi **ambiguous**.
- Mảng truyền vào hàm bị **decay** thành con trỏ tới phần tử đầu: `void f(int a[])` giống hệt `void f(int *a)`, mất kích thước → phải truyền thêm size. `const int a[]` để hàm không sửa được mảng.
- **Scope**: local (khối `{}`), global, static local (khởi tạo một lần, sống suốt chương trình nhưng chỉ nhìn thấy trong hàm).
- **Cơ chế gọi hàm**: mỗi lần gọi đẩy một **stack frame** (activation record) gồm tham số, biến local, địa chỉ trả về. Hàm return thì frame bị gỡ. Stack có giới hạn (mặc định 8 MB trên Linux) → đệ quy quá sâu gây **stack overflow**.
- `inline`: gợi ý compiler chèn thân hàm vào chỗ gọi để bỏ chi phí gọi; compiler tự quyết. Hàm inline được định nghĩa trong header mà không vi phạm ODR.
- **Đệ quy**: phải có điều kiện dừng (base case). Ví dụ: factorial, fibonacci (bản ngây thơ có độ phức tạp O(2ⁿ)). Đệ quy nào cũng viết lại được thành vòng lặp.

### Con trỏ (section 12)
- Con trỏ là biến chứa **địa chỉ**. Khai báo `int *p;` (chưa khởi tạo → địa chỉ rác). Nên khởi tạo ngay: `int *p = nullptr;` (C++11, thay cho `NULL`/`0`).
- `&x`: lấy địa chỉ. `*p`: dereference (truy cập giá trị tại địa chỉ). `p->m` = `(*p).m`.
- `sizeof` mọi con trỏ dữ liệu đều như nhau (8 byte trên 64-bit), không phụ thuộc kiểu được trỏ tới.
- Kiểu con trỏ phải khớp: `double *p = &int_var;` lỗi biên dịch.
- **Cấp phát động**: `new T` / `delete p`; `new T[n]` / `delete[] p`. Không trộn `new[]` với `delete`, không trộn `new` với `free`. Vùng nhớ nằm trên **heap**, sống tới khi delete.
- **Mảng và con trỏ**: `a[i]` ≡ `*(a + i)`. `p + 1` tiến thêm `sizeof(*p)` byte. Hiệu hai con trỏ (cùng một mảng) = số phần tử giữa chúng (`ptrdiff_t`). So sánh hai con trỏ `==` là so sánh địa chỉ.
- **const và con trỏ**: `const int *p` (con trỏ tới hằng: không sửa `*p`), `int *const p` (con trỏ hằng: không đổi `p`), `const int *const p` (cả hai). Đọc từ phải sang trái.
- Truyền con trỏ vào hàm để hàm sửa được biến gốc (cách của C). Muốn hàm đổi **chính con trỏ** thì truyền `int *&p` hoặc `int **p`.
- Trả về con trỏ từ hàm: được nếu trỏ tới vùng nhớ vẫn còn sống (tham số con trỏ, vùng `new`, biến static); **không** được trả về địa chỉ biến local.
- **Lỗi hay gặp**: con trỏ chưa khởi tạo (wild pointer), **dangling pointer** (trỏ vào vùng đã delete hoặc biến đã hết scope), dereference `nullptr` (segfault), **memory leak** (mất con trỏ tới vùng chưa delete), double delete. `delete nullptr` an toàn → gán `p = nullptr` sau khi delete.

### Tham chiếu (reference)
- `int &r = x;` tạo tên khác cho x (alias). **Bắt buộc khởi tạo**, **không thể null**, **không thể gắn lại** sang biến khác: `r = y` là gán giá trị của y vào x.
- `sizeof(r)` = `sizeof(x)`; `&r == &x`.
- Dùng làm tham số (tránh copy, sửa biến gốc) và trong range-for (`for (auto &x : v)`).
- `const T&` gắn được vào giá trị tạm (rvalue): `const int &r = 5;` hợp lệ, còn `int &r = 5;` thì lỗi.
- **lvalue**: có tên, có địa chỉ, đứng được bên trái phép gán. **rvalue**: giá trị tạm (`5`, `x + 1`, kết quả hàm trả về theo giá trị). C++11 thêm rvalue reference `T&&` (xem move semantics).

| | Con trỏ | Tham chiếu |
| --- | --- | --- |
| Null | có thể | không |
| Khởi tạo | không bắt buộc | bắt buộc |
| Đổi đối tượng trỏ tới | được | không |
| Pointer arithmetic | có | không |
| Cú pháp truy cập | `*p`, `p->m` | như biến thường |
| Dùng khi | có thể không có đối tượng, cần trỏ lại, mảng động | tham số/alias luôn hợp lệ |

## Trắc nghiệm

### Có thể overload hai hàm chỉ khác nhau ở tên tham số không?
```cpp
void f(int a);
void f(int b);
```
- [ ] Được, vì tên tham số khác nhau
- [x] Không, đó là cùng một hàm (khai báo lại)
- [ ] Được nếu thêm `inline`
- [ ] Lỗi linker
> Chữ ký hàm chỉ gồm tên hàm và kiểu các tham số; tên tham số không tính.

### Với các overload dưới, `print("hi")` gọi bản nào?
```cpp
void print(bool);
void print(const std::string &);
```
- [x] `print(bool)`
- [ ] `print(const std::string &)`
- [ ] Lỗi ambiguous
- [ ] Lỗi biên dịch: không có bản phù hợp
> `const char*` → `bool` là chuyển đổi chuẩn, được ưu tiên hơn `const char*` → `std::string` (chuyển đổi do người dùng định nghĩa). Đây là bẫy thật; xem lab 21.

### Với `void f(int); void f(double);`, lời gọi `f(5L)` (long) cho kết quả gì?
- [ ] Gọi f(int)
- [ ] Gọi f(double)
- [x] Lỗi biên dịch: ambiguous
- [ ] Gọi f(int) vì long gần int hơn
> long → int và long → double đều là chuyển đổi chuẩn cùng hạng nên compiler không chọn được.

### `f(3.5f)` với `void f(int); void f(double);` gọi bản nào?
- [ ] f(int)
- [x] f(double)
- [ ] Ambiguous
- [ ] Lỗi biên dịch
> float → double là **promotion**, ưu tiên hơn float → int (chuyển đổi chuẩn).

### Khai báo tham số mặc định nào hợp lệ?
- [ ] `void f(int a = 1, int b);`
- [x] `void f(int a, int b = 2, int c = 3);`
- [ ] `void f(int a = 1, int b, int c = 3);`
- [ ] `void f(int = 1, int);`
> Tham số mặc định phải nằm liền nhau ở cuối danh sách.

### Prototype `void f(int x = 5);` ở header, định nghĩa trong .cpp nên viết thế nào?
- [ ] `void f(int x = 5) { ... }`
- [x] `void f(int x) { ... }`
- [ ] `void f(int x = 0) { ... }`
- [ ] `void f() { ... }`
> Ghi giá trị mặc định ở cả hai nơi sẽ báo lỗi "redefinition of default argument". Chỉ ghi ở khai báo mà nơi gọi nhìn thấy.

### Đoạn code in ra gì?
```cpp
int counter() { static int c = 0; return ++c; }
int main() { counter(); counter(); cout << counter(); }
```
- [ ] 1
- [ ] 0
- [x] 3
- [ ] Không xác định
> Biến static local chỉ khởi tạo ở lần gọi đầu tiên và giữ giá trị giữa các lần gọi.

### Bên trong `void f(int arr[])`, `sizeof(arr)` trên Linux 64-bit bằng?
- [ ] Kích thước cả mảng truyền vào
- [x] 8 (kích thước con trỏ)
- [ ] 4
- [ ] Lỗi biên dịch
> Tham số mảng decay thành `int*`. gcc còn cảnh báo `-Wsizeof-array-argument`. Phải truyền kích thước làm tham số riêng.

### Cách khai báo tham số nào tốt nhất cho hàm chỉ đọc một `std::vector<int>` lớn?
- [ ] `void f(std::vector<int> v)`
- [ ] `void f(std::vector<int> &v)`
- [x] `void f(const std::vector<int> &v)`
- [ ] `void f(std::vector<int> *v)`
> Không copy, không sửa được, và nhận được cả giá trị tạm.

### Điều gì xảy ra mỗi lần gọi một hàm?
- [ ] Hàm được copy lên heap
- [x] Một stack frame chứa tham số, biến local và địa chỉ trả về được đẩy lên call stack
- [ ] Biến global được lưu lại
- [ ] Tạo một thread mới
> Khi hàm return thì frame bị gỡ, nên biến local không còn hợp lệ.

### Hàm đệ quy không có điều kiện dừng sẽ gây ra?
- [ ] Lỗi biên dịch
- [ ] Vòng lặp vô hạn không ảnh hưởng gì
- [x] Stack overflow (thường là segfault)
- [ ] Memory leak trên heap
> Mỗi lần gọi tốn thêm một stack frame cho tới khi hết stack.

### `inline` trên một hàm có ý nghĩa gì?
- [ ] Bắt buộc compiler chèn thân hàm vào chỗ gọi
- [x] Gợi ý chèn thân hàm, và cho phép định nghĩa hàm trong header mà không vi phạm ODR
- [ ] Hàm chạy trên thread riêng
- [ ] Hàm không được gọi đệ quy
> Compiler có thể bỏ qua gợi ý (hàm lớn, đệ quy) hoặc tự inline hàm không có từ khoá này khi bật tối ưu.

### Hàm nào trả về giá trị nguy hiểm (dangling)?
- [ ] `int f() { int x = 5; return x; }`
- [x] `int &f() { int x = 5; return x; }`
- [ ] `int *f() { return new int{5}; }`
- [ ] `int *f() { static int x = 5; return &x; }`
> Trả về reference tới biến local: biến bị huỷ khi hàm return. Trả về theo giá trị thì luôn an toàn; con trỏ tới vùng `new` hoặc biến static vẫn còn sống.

### Đoạn code in ra gì?
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
> Reference không gắn lại được: `r = b` gán 20 vào a, rồi `r = 5` gán 5 vào a. b không đổi.

### Phát biểu nào ĐÚNG về reference? (chọn tất cả đáp án đúng)
- [x] Phải được khởi tạo khi khai báo
- [ ] Có thể gán bằng nullptr
- [x] Không thể đổi để tham chiếu tới đối tượng khác
- [x] `const int &r = 5;` hợp lệ
> Chỉ `const T&` (hoặc `T&&`) mới gắn được vào giá trị tạm như 5.

### `int &r = 5;` thì sao?
- [ ] Hợp lệ, r = 5
- [x] Lỗi biên dịch: không gắn non-const lvalue reference vào rvalue
- [ ] Undefined behavior
- [ ] Hợp lệ nhưng có cảnh báo
> 5 là rvalue. `const int &r = 5;` thì được: compiler tạo một biến tạm sống cùng r.

### Đoạn code in ra gì?
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
- [ ] Lỗi biên dịch
> Con trỏ được truyền theo giá trị: hàm chỉ đổi bản copy. Muốn đổi q phải dùng `int *&p` hoặc `int **p`.

### Với `int a[] = {10, 20, 30}; int *p = a;`, biểu thức `*p + 1` và `*(p + 1)` bằng?
- [ ] 20 và 20
- [x] 11 và 20
- [ ] 11 và 11
- [ ] 20 và 11
> `*` ưu tiên hơn `+`: `*p + 1` = 10 + 1. `*(p + 1)` là phần tử thứ hai.

### Với `double d[10]; double *p = &d[2], *q = &d[7];`, `q - p` bằng?
- [ ] 40
- [x] 5
- [ ] 20
- [ ] Lỗi biên dịch
> Hiệu hai con trỏ tính theo số phần tử, không theo byte.

### Khai báo nào cho phép đổi con trỏ sang địa chỉ khác nhưng KHÔNG cho sửa giá trị được trỏ tới?
- [x] `const int *p`
- [ ] `int *const p`
- [ ] `const int *const p`
- [ ] `int const *const p`
> Đọc từ phải sang trái: `p` là con trỏ tới `const int`. `int const *p` giống hệt `const int *p`.

### Cặp cấp phát/giải phóng nào ĐÚNG?
- [ ] `int *p = new int[10]; delete p;`
- [x] `int *p = new int[10]; delete[] p;`
- [ ] `int *p = new int; free(p);`
- [ ] `int *p = (int*)malloc(4); delete p;`
> new đi với delete, new[] đi với delete[], malloc đi với free. Trộn lẫn là undefined behavior.

### Đoạn code có vấn đề gì?
```cpp
void f() {
    int *p = new int[100];
    if (error()) return;
    delete[] p;
}
```
- [ ] Không có vấn đề
- [x] Memory leak khi `error()` trả về true
- [ ] Double free
- [ ] Dangling pointer
> Return sớm bỏ qua delete. Cách sửa trong C++ hiện đại: `std::vector<int>` hoặc `std::unique_ptr<int[]>` (RAII).

### `delete p;` rồi dùng `*p` tiếp là lỗi gì?
- [ ] Memory leak
- [x] Dangling pointer (use-after-free), undefined behavior
- [ ] Null pointer dereference
- [ ] Lỗi biên dịch
> Sau delete nên gán `p = nullptr`; dùng nullptr sẽ crash ngay thay vì đọc rác âm thầm. AddressSanitizer (`-fsanitize=address`) phát hiện được lỗi này.

### `sizeof(int*)` và `sizeof(double*)` trên Linux x86-64?
- [ ] 4 và 8
- [x] 8 và 8
- [ ] 4 và 4
- [ ] Phụ thuộc giá trị trỏ tới
> Kích thước con trỏ chỉ phụ thuộc kiến trúc, không phụ thuộc kiểu được trỏ tới.

### Biểu thức nào là rvalue?
- [ ] `x`
- [ ] `arr[2]`
- [x] `x + 1`
- [ ] `*p`
> `x + 1` là giá trị tạm, không có địa chỉ. `x`, `arr[2]`, `*p` đều chỉ tới một vùng nhớ cụ thể (lvalue) và đứng được bên trái phép gán.

### Khi nào nên dùng con trỏ thay vì reference? (chọn tất cả đáp án đúng)
- [x] Khi đối tượng có thể không tồn tại (cần giá trị null)
- [x] Khi cần trỏ sang đối tượng khác về sau
- [ ] Khi muốn hàm nhận tham số không bị copy
- [x] Khi làm việc với mảng động cấp phát bằng new[]
> Chỉ muốn tránh copy thì reference (đặc biệt `const T&`) gọn và an toàn hơn.
