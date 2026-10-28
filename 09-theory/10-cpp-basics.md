# C++ cơ bản: biến, toán tử, điều khiển luồng, chuỗi

## Lý thuyết

Theo Udemy *Beginning C++ Programming*, section 4–10. Lab: `02-cpp/17` … `02-cpp/20`.

### Các loại lỗi (section 4)
| Loại | Khi nào phát hiện | Ví dụ |
| --- | --- | --- |
| Compiler error (syntax/semantic) | lúc biên dịch | thiếu `;`, dùng biến chưa khai báo, gán `string` vào `int` |
| Compiler warning | lúc biên dịch, vẫn tạo được file chạy | biến chưa khởi tạo, so sánh signed/unsigned, biến không dùng |
| Linker error | lúc liên kết | khai báo hàm nhưng không định nghĩa (`undefined reference`), định nghĩa trùng (`multiple definition`) |
| Runtime error | lúc chạy | chia cho 0, mở file không tồn tại, truy cập con trỏ null → crash |
| Logic error | không ai báo, kết quả sai | dùng `>` thay vì `>=`, chia nguyên khi cần chia thực |

Luôn bật `-Wall -Wextra` và coi warning như lỗi tiềm ẩn.

### Cấu trúc chương trình (section 5)
- `#include <iostream>`: chỉ thị **preprocessor**, chạy trước khi biên dịch; dán nội dung header vào file. `<...>` tìm trong thư mục hệ thống, `"..."` tìm ở thư mục hiện tại trước.
- Comment: `//` một dòng, `/* ... */` nhiều dòng (không lồng nhau được). Comment bị bỏ đi ở bước preprocessor.
- `main()`: điểm bắt đầu; `int main()` hoặc `int main(int argc, char *argv[])`. `argv[0]` là tên chương trình. Thiếu `return` trong main thì mặc định trả về 0; trả về 0 nghĩa là thành công.
- Namespace: tránh trùng tên. `std::cout`, `using std::cout;` (chỉ một tên), `using namespace std;` (cả namespace; không nên dùng trong header).
- `cout << ` (insertion operator), `cin >> ` (extraction operator). `cin >>` bỏ qua khoảng trắng và dừng ở khoảng trắng. `endl` = xuống dòng + flush; `'\n'` chỉ xuống dòng.
- `cerr`: không có buffer; `clog`: có buffer; cả hai ghi ra stderr.

### Biến và hằng (section 6)
- Biến: tên + kiểu + vùng nhớ. Tên bắt đầu bằng chữ hoặc `_`, phân biệt hoa thường, không trùng từ khoá.
- Ba cách khởi tạo: `int a = 5;` (C-like), `int a(5);` (constructor), `int a{5};` (list/uniform, C++11, **chặn narrowing**: `int x{3.5}` lỗi).
- Biến local chưa khởi tạo chứa giá trị rác; biến global/static được khởi tạo về 0.
- Biến global: khai báo ngoài mọi hàm, nhìn thấy ở mọi nơi sau khai báo. Nên tránh dùng.
- Kiểu cơ bản: `char` (1 byte), `short` (≥ 2), `int` (≥ 2, thường 4), `long` (≥ 4; 8 trên Linux 64-bit), `long long` (≥ 8), `float` (4, ~7 chữ số), `double` (8, ~15 chữ số), `long double`, `bool` (thường 1 byte).
- `signed`/`unsigned`: unsigned không có số âm, tràn thì quay vòng (định nghĩa rõ); signed tràn là UB.
- `sizeof` là **toán tử** (tính lúc biên dịch), trả về `size_t`. Giới hạn kiểu: `<climits>` (`INT_MAX`), `<cfloat>`, `std::numeric_limits<int>::max()`.
- Hằng: literal (`12`, `12U`, `12L`, `12.0f`, `'A'`, `"abc"`), `const` (không đổi được sau khởi tạo, phải khởi tạo ngay), `constexpr` (tính lúc biên dịch), `enum`, `#define` (thay thế text, không có kiểu, **không nên dùng** trong C++).

### Mảng và vector (section 7)
- Mảng: kích thước cố định, biết lúc biên dịch, phần tử liền nhau, chỉ số 0 … n-1, **không kiểm tra biên**. `int a[5] {1, 2};` → các phần tử còn lại là 0. `int a[] {1, 2, 3};` → tự suy ra kích thước 3.
- Tên mảng là địa chỉ phần tử đầu. Không gán mảng cho mảng (`a = b` lỗi).
- Mảng nhiều chiều: `int m[3][4];` lưu theo hàng (row-major).
- `std::vector` (`#include <vector>`): mảng động, tự quản lý bộ nhớ. `vector<int> v(5);` = 5 phần tử 0; `vector<int> v{5};` = 1 phần tử có giá trị 5; `vector<int> v(5, 1);` = 5 phần tử 1.
- `v[i]` không kiểm tra biên, `v.at(i)` kiểm tra và ném `std::out_of_range`. `push_back`, `size()`, `vector<vector<int>>` cho 2 chiều.

### Biểu thức và toán tử (section 8)
- Expression: thứ tính ra giá trị. Statement: câu lệnh hoàn chỉnh kết thúc bằng `;`.
- Toán tử gán `=` trả về lvalue vế trái → `a = b = 5` gán từ phải sang trái. `=` không phải `==`.
- Số học `+ - * / %`. `int / int` là chia nguyên (cắt về 0). `%` chỉ dùng cho số nguyên.
- `++i` (tăng rồi dùng) và `i++` (dùng rồi tăng). Không sửa cùng một biến hai lần trong một biểu thức (`i = i++ + 1` là UB trước C++17).
- Mixed expression: kiểu nhỏ được nâng lên kiểu lớn hơn (promotion: `short`/`char` → `int`, `int` → `double`). Gán double vào int thì cắt phần thập phân (demotion). Ép kiểu tường minh: `static_cast<double>(x)`, kiểu C `(double)x`.
- So sánh `== != < > <= >=` (C++20 có `<=>`). Không so sánh số thực bằng `==`.
- Logic: `!` (cao nhất), `&&`, `||` (thấp nhất). **Short-circuit**: `&&` dừng khi gặp false, `||` dừng khi gặp true.
- Gán kép `+= -= *= /= %= <<= >>= &= ^= |=`: `a *= b + c` là `a = a * (b + c)`.
- Độ ưu tiên (cao → thấp): `::` → hậu tố `a++ a() a[] .` `->` → tiền tố `++a !a -a *p &a sizeof` cast → `* / %` → `+ -` → `<< >>` → `< <= > >=` → `== !=` → `&` → `^` → `|` → `&&` → `||` → `?:` và gán `= +=` … → `,`. Gán và `?:` kết hợp **phải sang trái**, hầu hết toán tử khác trái sang phải.

### Điều khiển luồng (section 9)
- `if`, `if-else`, if lồng nhau: `else` gắn với `if` gần nhất chưa có else (dangling else) → nên luôn dùng `{}`.
- `switch (expr)`: expr phải là số nguyên, char hoặc enum (không string, không double). Nhãn `case` phải là hằng lúc biên dịch. Thiếu `break` → chạy tiếp xuống case sau (fall-through). `default` không bắt buộc. Không khai báo biến có khởi tạo trong case mà không có `{}`.
- Toán tử ba ngôi: `cond ? a : b`, là **biểu thức** (có giá trị) nên dùng được trong `cout`.
- `for (init; cond; update)`: init và update có thể có nhiều biến (dấu phẩy). `for (;;)` là vòng lặp vô hạn.
- Range-based for (C++11): `for (auto x : v)` copy, `for (auto &x : v)` sửa được, `for (const auto &x : v)` đọc không copy.
- `while` kiểm tra trước (có thể 0 lần); `do { } while (cond);` kiểm tra sau (ít nhất 1 lần, nhớ dấu `;` cuối).
- `break` thoát vòng lặp/switch gần nhất; `continue` sang lần lặp tiếp của vòng gần nhất.
- Vòng lặp vô hạn: `while (true)`, `for (;;)`; thoát bằng `break`. Hay dùng cho vòng menu, event loop.

### Ký tự và chuỗi (section 10)
- `<cctype>`: `isalpha`, `isdigit`, `isalnum`, `isspace`, `isupper`, `islower`, `ispunct`, `toupper`, `tolower`. Hàm `is...` trả về int khác 0 (không nhất thiết là 1).
- C-style string: mảng `char` kết thúc bằng `'\0'`. `char s[] = "Hi";` có kích thước 3. String literal `"Hi"` là `const char[3]`, không được sửa.
- `<cstring>`: `strlen` (không đếm `'\0'`), `strcpy`, `strcat` (không kiểm tra tràn), `strcmp` (0 = bằng, < 0, > 0), `strncpy`. `<cstdlib>`: `atoi`, `atof`.
- `std::string` (`#include <string>`): tự quản lý bộ nhớ, so sánh bằng `== < >` theo thứ tự từ điển, nối bằng `+` (ít nhất một vế phải là `std::string`: `"a" + "b"` lỗi).
- Hàm của string: `size()`/`length()`, `substr(pos, len)`, `find(s)` (không thấy → `string::npos`), `rfind`, `erase(pos, len)`, `insert(pos, s)`, `replace`, `clear()`, `empty()`, `at(i)` (kiểm tra biên), `c_str()` (lấy `const char*`), `std::stoi`, `std::to_string`.
- `cin >> s` đọc một từ; `getline(cin, s)` đọc cả dòng (bỏ `'\n'`). Gọi `cin >> n` rồi `getline` ngay sau thì getline đọc phải `'\n'` còn sót → chuỗi rỗng; sửa bằng `cin.ignore()` hoặc `cin >> ws`.

## Trắc nghiệm

### Quên định nghĩa một hàm đã khai báo và có gọi, lỗi xuất hiện ở giai đoạn nào?
- [ ] Preprocessor
- [ ] Biên dịch (compiler error)
- [x] Liên kết (linker error: undefined reference)
- [ ] Lúc chạy
> Khai báo đủ để compiler biên dịch lời gọi. Chỉ đến khi linker ghép các file object mới phát hiện không có định nghĩa.

### Chương trình tính trung bình in ra 2 thay vì 2.33. Đây là loại lỗi gì?
```cpp
int a = 1, b = 2, c = 4;
double avg = (a + b + c) / 3;
```
- [ ] Compiler error
- [ ] Runtime error
- [x] Logic error
- [ ] Linker error
> Code hợp lệ và chạy bình thường, chỉ là kết quả sai vì chia nguyên. Sửa thành `/ 3.0` hoặc `static_cast<double>(a + b + c) / 3`.

### `#include` được xử lý bởi?
- [x] Preprocessor, trước khi biên dịch
- [ ] Compiler, trong lúc biên dịch
- [ ] Linker
- [ ] Hệ điều hành khi chạy chương trình
> Preprocessor xử lý mọi dòng bắt đầu bằng `#` (include, define, ifdef) và xoá comment, rồi mới đưa kết quả cho compiler.

### Khác nhau giữa `#include <file>` và `#include "file"`?
- [ ] Không khác gì
- [x] `"file"` tìm ở thư mục của file nguồn trước, rồi mới tới thư mục hệ thống; `<file>` chỉ tìm ở thư mục hệ thống/thư mục -I
- [ ] `<file>` dùng cho C, `"file"` dùng cho C++
- [ ] `"file"` chỉ dùng cho file .cpp
> Quy ước: header thư viện dùng `<>`, header của dự án dùng `""`.

### Vì sao không nên viết `using namespace std;` trong file header?
- [ ] Vì làm chương trình chạy chậm
- [x] Vì mọi file include header đó đều bị kéo toàn bộ tên của std vào, dễ trùng tên
- [ ] Vì header không được dùng namespace
- [ ] Vì cú pháp đó chỉ hợp lệ trong main
> `using` không tốn gì lúc chạy. Vấn đề là ô nhiễm namespace ở mọi nơi include header đó.

### `std::endl` khác `'\n'` ở điểm nào?
- [ ] `endl` xuống 2 dòng
- [x] `endl` xuống dòng và flush buffer của stream
- [ ] `'\n'` không xuống dòng trên Linux
- [ ] Không khác gì
> Flush liên tục làm chậm khi in nhiều. Dùng `'\n'` trừ khi cần đẩy output ra ngay.

### Khởi tạo nào báo lỗi biên dịch?
- [ ] `int a = 3.7;`
- [ ] `int a(3.7);`
- [x] `int a{3.7};`
- [ ] `double a{3};`
> List initialization `{}` cấm narrowing (double → int mất dữ liệu). `=` và `()` chỉ cắt phần thập phân, cùng lắm là cảnh báo. `double a{3}` là mở rộng (int → double) nên hợp lệ.

### Giá trị của biến local `int x;` chưa khởi tạo là gì?
- [ ] Luôn là 0
- [x] Không xác định (rác); đọc nó là undefined behavior
- [ ] -1
- [ ] Lỗi biên dịch
> Chỉ biến global/static mới tự khởi tạo về 0. Compiler thường cảnh báo `-Wuninitialized`/`-Wmaybe-uninitialized`.

### `sizeof` là gì?
- [ ] Một hàm trong `<cstddef>`
- [x] Một toán tử, kết quả có kiểu `size_t` và thường tính lúc biên dịch
- [ ] Một macro
- [ ] Một hàm của class
> Vì là toán tử nên `sizeof x` (không ngoặc) hợp lệ với biến; với kiểu thì phải có ngoặc: `sizeof(int)`.

### Điều nào ĐÚNG về `const` và `constexpr`? (chọn tất cả đáp án đúng)
- [x] Biến `const` phải được khởi tạo khi khai báo
- [x] `constexpr` bắt buộc giá trị tính được lúc biên dịch
- [ ] `const int n = rand();` báo lỗi
- [x] `constexpr int n = 10;` dùng được làm kích thước mảng
> `const` chỉ cấm sửa, giá trị có thể có lúc chạy (`rand()` hợp lệ). `constexpr` mạnh hơn: phải là hằng lúc biên dịch.

### Vì sao C++ khuyên dùng `const`/`constexpr` thay cho `#define PI 3.14`?
- [ ] `#define` chạy chậm hơn
- [x] `#define` chỉ thay thế text: không có kiểu, không có phạm vi, không thấy được khi debug
- [ ] `#define` không dùng được cho số thực
- [ ] `const` được lưu trong thanh ghi
> Macro bị thay thế trước khi compiler thấy, nên không có kiểm tra kiểu, không có scope và dễ dính lỗi do thiếu ngoặc.

### `int a[5] {1, 2};` thì `a[4]` bằng?
- [ ] Rác
- [x] 0
- [ ] 2
- [ ] Lỗi biên dịch
> Khi có initializer list, các phần tử không được ghi rõ được khởi tạo về 0.

### Khác nhau giữa `vector<int> v(5);` và `vector<int> v{5};`?
- [ ] Giống nhau: đều là 5 phần tử
- [x] `(5)` là 5 phần tử giá trị 0; `{5}` là 1 phần tử giá trị 5
- [ ] `(5)` là 1 phần tử giá trị 5; `{5}` là 5 phần tử
- [ ] `{5}` lỗi biên dịch
> Dấu `{}` ưu tiên constructor nhận `initializer_list`.

### `v.at(10)` với vector có 3 phần tử sẽ?
- [ ] Trả về 0
- [ ] Undefined behavior
- [x] Ném `std::out_of_range`
- [ ] Tự mở rộng vector
> `at()` kiểm tra biên. `v[10]` thì không kiểm tra → UB.

### Kết quả của `-7 / 2` và `-7 % 2` trong C++11?
- [ ] -4 và 1
- [x] -3 và -1
- [ ] -3 và 1
- [ ] -4 và -1
> Chia nguyên cắt về 0, và `(a / b) * b + a % b == a` → `-7 % 2 = -7 - (-3 * 2) = -1`.

### Đoạn code in ra gì?
```cpp
int i = 5;
int j = i++ + 10;
cout << i << ' ' << j;
```
- [ ] 5 15
- [x] 6 15
- [ ] 6 16
- [ ] 5 16
> `i++` trả về giá trị cũ (5) để tính j = 15, sau đó i thành 6.

### `a *= 2 + 3;` với a = 4, a bằng?
- [ ] 11
- [x] 20
- [ ] 8
- [ ] 14
> Gán kép có độ ưu tiên thấp nhất: `a = a * (2 + 3)` = 20.

### Kết quả của `(5 > 3 > 1)`?
- [ ] true, vì 5 > 3 và 3 > 1
- [x] false
- [ ] Lỗi biên dịch
- [ ] Undefined behavior
> Tính trái sang phải: `(5 > 3)` = true = 1, rồi `1 > 1` = false.

### Đoạn code in ra gì?
```cpp
int x = 0, y = 0;
if (x != 0 && ++y > 0) {}
if (x == 0 || ++y > 0) {}
cout << y;
```
- [x] 0
- [ ] 1
- [ ] 2
- [ ] Không xác định
> Cả hai lần đều short-circuit: vế trái của `&&` false, vế trái của `||` true → `++y` không lần nào chạy.

### `true || false && false` bằng?
- [x] true
- [ ] false
- [ ] Lỗi biên dịch
- [ ] Phụ thuộc compiler
> `&&` ưu tiên hơn `||`: `true || (false && false)` = true.

### `static_cast<double>(7 / 2)` bằng?
- [ ] 3.5
- [x] 3.0
- [ ] 4.0
- [ ] Lỗi biên dịch
> Biểu thức trong ngoặc là chia nguyên (3), sau đó mới ép sang double. Muốn 3.5 thì ép trước: `static_cast<double>(7) / 2`.

### `cout << 'A' + 1;` in ra?
- [ ] B
- [x] 66
- [ ] A1
- [ ] Lỗi biên dịch
> `char` được nâng lên `int` trong phép cộng → kết quả là số 66. Muốn in 'B' phải ép `char('A' + 1)`.

### Kiểu nào KHÔNG dùng được làm biểu thức điều kiện của `switch`?
- [ ] `char`
- [ ] `enum`
- [x] `std::string`
- [ ] `long`
> switch chỉ nhận kiểu số nguyên (gồm char, bool) và enum. String và số thực phải dùng chuỗi if-else.

### Đoạn code in ra gì?
```cpp
int n = 2;
switch (n) {
case 1: cout << "A";
case 2: cout << "B";
case 3: cout << "C"; break;
default: cout << "D";
}
```
- [ ] B
- [x] BC
- [ ] BCD
- [ ] BD
> Không có `break` sau case 2 nên chạy tiếp vào case 3 (fall-through), rồi `break` ở case 3 dừng lại.

### Đoạn code in ra gì?
```cpp
int x = 1, y = 0;
if (x)
    if (y) cout << "A";
else
    cout << "B";
```
- [ ] A
- [x] B
- [ ] Không in gì
- [ ] AB
> `else` gắn với `if (y)` gần nhất, thụt lề không có ý nghĩa với compiler.

### Vòng lặp nào luôn chạy thân ít nhất một lần?
- [ ] `for`
- [ ] `while`
- [x] `do-while`
- [ ] range-based for
> do-while kiểm tra điều kiện ở cuối mỗi lần lặp.

### Sau đoạn code, v là gì?
```cpp
vector<int> v{1, 2, 3};
for (auto x : v) x *= 2;
```
- [x] {1, 2, 3}
- [ ] {2, 4, 6}
- [ ] Lỗi biên dịch
- [ ] {1, 4, 9}
> `auto x` là bản copy. Muốn sửa phần tử phải dùng `auto &x`.

### `break` trong vòng lặp lồng nhau thoát khỏi?
- [x] Chỉ vòng lặp trong cùng chứa nó
- [ ] Tất cả các vòng lặp
- [ ] Cả hàm
- [ ] Vòng lặp ngoài cùng
> Muốn thoát nhiều vòng thì dùng cờ, tách thành hàm rồi `return`, hoặc `goto`.

### `sizeof` của `char s[] = "Hello";` và `strlen(s)` lần lượt là?
- [ ] 5 và 5
- [x] 6 và 5
- [ ] 5 và 6
- [ ] 8 và 5
> Mảng chứa 5 ký tự + `'\0'`. strlen dừng trước `'\0'`.

### Cách đúng để so sánh nội dung hai C-style string `s1` và `s2`?
- [ ] `s1 == s2`
- [x] `strcmp(s1, s2) == 0`
- [ ] `strcmp(s1, s2) == 1`
- [ ] `s1.compare(s2)`
> `==` trên `char*`/mảng so sánh địa chỉ. strcmp trả về 0 khi bằng nhau, số âm/dương khi nhỏ hơn/lớn hơn.

### Dòng nào báo lỗi biên dịch?
- [ ] `string s = string("a") + "b";`
- [ ] `string s = "a"; s += "b";`
- [x] `string s = "a" + "b";`
- [ ] `string s = "a" + string("b");`
> `"a" + "b"` là cộng hai con trỏ `const char*`, không được phép. Phải có ít nhất một vế là `std::string`.

### `s.find("xyz")` khi không tìm thấy trả về?
- [ ] -1 kiểu int
- [ ] 0
- [x] `std::string::npos`
- [ ] Ném exception
> `npos` là giá trị lớn nhất của `size_t`. Luôn so sánh `if (s.find(x) != string::npos)`.

### `string("Zebra") < string("apple")` bằng?
- [x] true
- [ ] false
- [ ] Lỗi biên dịch
- [ ] Không xác định
> So sánh theo mã ASCII từng ký tự: 'Z' (90) < 'a' (97).

### Người dùng nhập `42` rồi Enter. Đoạn code làm gì?
```cpp
int n; string name;
cin >> n;
getline(cin, name);
```
- [ ] Chờ nhập tên
- [x] `name` là chuỗi rỗng, không chờ nhập
- [ ] `name` là "42"
- [ ] Lỗi lúc chạy
> `cin >> n` để lại `'\n'` trong buffer, getline đọc ngay ký tự đó. Gọi `cin.ignore()` (hoặc `getline(cin >> ws, name)`) trước.
