# STL và C++ hiện đại (11/14/17)

## Lý thuyết

### Container và độ phức tạp
| Container | Cấu trúc | Truy cập | Chèn/xoá | Ghi chú |
| --- | --- | --- | --- | --- |
| `vector` | mảng động liên tục | O(1) | cuối O(1) khấu hao, giữa O(n) | tăng capacity thường ×2 (libstdc++) hoặc ×1.5 (MSVC) |
| `deque` | các khối | O(1) | hai đầu O(1) | |
| `list` | danh sách liên kết đôi | O(n) | O(1) khi có iterator | iterator không bị vô hiệu khi chèn |
| `map` / `set` | cây đỏ-đen | O(log n) | O(log n) | có thứ tự theo key |
| `unordered_map` / `unordered_set` | bảng băm | O(1) trung bình, O(n) xấu nhất | O(1) trung bình | không thứ tự, rehash khi đầy |
| `array` | mảng cố định | O(1) | — | kích thước biết lúc biên dịch |
| `stack`, `queue`, `priority_queue` | adapter | | | priority_queue là heap, mặc định max-heap |

- `map[key]` **tự chèn** phần tử mặc định nếu chưa có; dùng `find`/`count`/`contains` (C++20) để kiểm tra.
- `size()` là số phần tử, `capacity()` là chỗ đã cấp phát. `reserve(n)` chỉ tăng capacity; `resize(n)` đổi size.
- Iterator invalidation: vector cấp phát lại → **mọi** iterator/pointer/reference vô hiệu; erase giữa vector → iterator từ vị trí đó trở đi vô hiệu. Xoá trong vòng lặp: `it = v.erase(it);`.
- Erase-remove idiom: `v.erase(remove(v.begin(), v.end(), x), v.end());` (C++20: `std::erase(v, x)`).

### Smart pointer (`<memory>`)
- `unique_ptr<T>`: sở hữu độc quyền, không copy được, chỉ move. Chi phí bằng con trỏ thường. Tạo bằng `make_unique` (C++14).
- `shared_ptr<T>`: sở hữu chung, đếm tham chiếu (atomic) trong control block; object bị huỷ khi count về 0. `make_shared` cấp phát object và control block một lần.
- `weak_ptr<T>`: tham chiếu không sở hữu, không tăng count; dùng `lock()` để lấy shared_ptr. Dùng để **phá vòng tham chiếu** (A giữ shared_ptr tới B, B giữ shared_ptr tới A → không bao giờ giải phóng).
- Không tạo hai shared_ptr từ cùng một con trỏ thô (hai control block → double delete).

### Move semantics
- lvalue: có tên/địa chỉ. rvalue: giá trị tạm. `T&&` là rvalue reference.
- `std::move(x)` chỉ là `static_cast<T&&>(x)` — không tự di chuyển gì; object bị move ở trạng thái hợp lệ nhưng không xác định.
- vector chỉ dùng move khi cấp phát lại nếu move ctor là `noexcept`.
- Perfect forwarding: `template<class T> void f(T&& x) { g(std::forward<T>(x)); }` (forwarding reference).
- Copy elision: C++17 bảo đảm khi trả về prvalue; NRVO cho biến có tên thì không bảo đảm nhưng compiler thường làm.

### Lambda
`[capture](params) mutable -> ret { body }`
- `[=]` copy mọi biến dùng tới (giá trị chụp lúc **tạo** lambda), `[&]` tham chiếu, `[x, &y]`, `[this]`, `[*this]` (C++17), init-capture `[p = std::move(ptr)]` (C++14).
- Lambda capture theo giá trị là `const` bên trong, cần `mutable` để sửa bản copy.
- Capture theo reference rồi dùng lambda sau khi biến hết scope → dangling.

### Tính năng khác
- `auto`, range-for, `nullptr` (kiểu `nullptr_t`, thay NULL), `enum class` (có phạm vi, không chuyển ngầm sang int), `constexpr` (tính lúc biên dịch), `static_assert`, `using` alias, `= default/= delete`, `override/final`, uniform initialization `{}` (chặn narrowing), structured binding `auto [a, b] = pair;` (C++17), `std::optional`, `std::variant`, `std::string_view` (C++17).
- Template: định nghĩa phải nằm trong header. Hàm thường ưu tiên hơn template khi khớp như nhau.

## Trắc nghiệm

### Độ phức tạp trung bình của tìm kiếm trong `std::map` và `std::unordered_map`?
- [ ] O(1) và O(1)
- [x] O(log n) và O(1)
- [ ] O(log n) và O(log n)
- [ ] O(n) và O(1)
> map là cây đỏ-đen cân bằng; unordered_map là bảng băm (xấu nhất O(n) khi nhiều va chạm).

### Đoạn sau in ra gì?
```cpp
std::map<std::string, int> m;
if (m["x"] == 0) {}
std::cout << m.size();
```
- [ ] 0
- [x] 1
- [ ] Ném exception
- [ ] Undefined behavior
> operator[] chèn phần tử mặc định khi key chưa có. Dùng `m.find("x")` hoặc `m.count("x")` để chỉ kiểm tra. `m.at("x")` ném `std::out_of_range`.

### Sau khi `push_back` làm vector cấp phát lại vùng nhớ, các iterator/con trỏ cũ tới phần tử thì sao?
- [ ] Vẫn hợp lệ
- [x] Bị vô hiệu (dùng là UB)
- [ ] Tự cập nhật sang vùng mới
- [ ] Chỉ iterator end() bị vô hiệu
> Khi capacity tăng, các phần tử được chuyển sang vùng mới và vùng cũ bị giải phóng.

### Cách xoá phần tử chẵn trong vector khi đang duyệt bằng iterator?
- [ ] `for (auto it = v.begin(); it != v.end(); ++it) if (*it % 2 == 0) v.erase(it);`
- [x] `for (auto it = v.begin(); it != v.end();) if (*it % 2 == 0) it = v.erase(it); else ++it;`
- [ ] `for (int x : v) if (x % 2 == 0) v.erase(x);`
- [ ] Không thể xoá khi đang duyệt
> erase trả iterator tới phần tử kế tiếp; iterator cũ đã bị vô hiệu.

### `v.reserve(100)` với vector rỗng thì `size()` và `capacity()` là?
- [ ] 100 và 100
- [x] 0 và ít nhất 100
- [ ] 100 và 0
- [ ] 0 và 0
> reserve chỉ cấp phát chỗ, không tạo phần tử. resize mới đổi size.

### Phát biểu nào đúng về `std::unique_ptr`?
- [ ] Có thể copy, khi đó cả hai cùng sở hữu
- [x] Không copy được, chỉ chuyển quyền sở hữu bằng move
- [ ] Có bộ đếm tham chiếu
- [ ] Không tự giải phóng bộ nhớ
> Copy constructor bị delete. `auto q = std::move(p);` → p thành nullptr.

### Đoạn sau in ra gì?
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
> weak_ptr không làm tăng use_count.

### Vấn đề nào `weak_ptr` dùng để giải quyết?
- [ ] Rò rỉ khi quên delete
- [x] Vòng tham chiếu giữa các shared_ptr làm object không bao giờ được giải phóng
- [ ] Double free khi copy unique_ptr
- [ ] Truy cập đồng thời từ nhiều thread
> Ví dụ cây có con giữ shared_ptr tới cha và cha giữ shared_ptr tới con → một chiều phải là weak_ptr.

### `std::move(x)` thực chất làm gì?
- [ ] Copy x sang vùng nhớ mới rồi xoá x
- [x] Ép x sang rvalue reference để có thể chọn move constructor/assignment
- [ ] Giải phóng bộ nhớ của x
- [ ] Đổi địa chỉ của x
> Việc di chuyển thật sự do move constructor/assignment của kiểu đó thực hiện.

### Đoạn sau in ra gì?
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
> Capture theo giá trị chụp lúc tạo lambda; capture theo reference đọc giá trị hiện tại.

### Muốn sửa biến capture theo giá trị bên trong lambda, cần gì?
- [ ] `const`
- [x] `mutable`
- [ ] `volatile`
- [ ] `static`
> operator() của lambda mặc định là const. mutable chỉ sửa bản copy bên trong lambda, không ảnh hưởng biến bên ngoài.

### Ưu điểm của `nullptr` so với `NULL`?
- [ ] Nhanh hơn
- [x] Có kiểu riêng (nullptr_t) nên không bị nhầm với số nguyên khi overload
- [ ] Có thể dereference an toàn
- [ ] Tự giải phóng bộ nhớ
> `f(int)` và `f(char*)`: `f(NULL)` có thể gọi f(int); `f(nullptr)` luôn gọi bản con trỏ.

### Khác biệt chính của `enum class` so với `enum` thường? (chọn tất cả đáp án đúng)
- [x] Tên hằng nằm trong phạm vi của enum (`Color::Red`)
- [x] Không tự chuyển ngầm sang int
- [ ] Không chỉ định được kiểu nền
- [ ] Chỉ dùng được trong class
> Có thể chỉ định kiểu nền: `enum class Color : uint8_t { ... }`.

### Container nào phù hợp nhất khi cần chèn/xoá thường xuyên ở giữa và giữ iterator không bị vô hiệu?
- [ ] vector
- [ ] deque
- [x] list
- [ ] array
> Chèn/xoá ở list không làm vô hiệu iterator tới phần tử khác. Tuy vậy thực tế vector thường nhanh hơn với dữ liệu nhỏ nhờ cache.

### `std::priority_queue<int>` mặc định lấy ra phần tử nào trước?
- [x] Lớn nhất
- [ ] Nhỏ nhất
- [ ] Phần tử chèn đầu tiên
- [ ] Phần tử chèn cuối cùng
> Max-heap. Muốn min-heap: `priority_queue<int, vector<int>, greater<int>>`.

### `constexpr` nghĩa là gì?
- [ ] Biến không thể thay đổi lúc chạy, giống const
- [x] Giá trị/hàm có thể (hoặc phải, với biến) được tính lúc biên dịch
- [ ] Hàm được inline bắt buộc
- [ ] Biến được đặt vào ROM
> Biến constexpr phải là hằng lúc biên dịch; hàm constexpr được tính lúc biên dịch khi tham số là hằng.

### Vì sao nên dùng `make_shared` thay vì `shared_ptr<T>(new T)`?
- [ ] Vì `new T` không hợp lệ với shared_ptr
- [x] Cấp phát object và control block trong một lần, nhanh hơn và an toàn với exception
- [ ] Vì make_shared không đếm tham chiếu
- [ ] Vì make_shared tạo unique_ptr
> Nhược điểm nhỏ: bộ nhớ của object chỉ được trả khi cả weak_ptr cũng hết.

### `auto [k, v] = *m.begin();` (với m là std::map) là tính năng gì?
- [ ] Lambda capture
- [x] Structured binding (C++17)
- [ ] Initializer list
- [ ] Template deduction
> Tách pair/tuple/struct thành các biến có tên.
