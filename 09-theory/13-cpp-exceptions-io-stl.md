# Smart pointer, exception, I/O stream, STL, lambda, enum

## Lý thuyết

Theo Udemy *Beginning C++ Programming*, section 17–21 và 23. Lab: `02-cpp/26` … `02-cpp/29`, xem thêm `02-cpp/06`, `07`, `08`, `11`. Chủ đề "STL và C++ hiện đại" ở trên có bảng độ phức tạp container và move semantics.

### Smart pointer (section 17)
- Vấn đề của con trỏ thô: quên delete (leak), delete hai lần, dangling, không rõ ai sở hữu, exception làm bỏ qua delete.
- **RAII** (Resource Acquisition Is Initialization): tài nguyên gắn với vòng đời object; constructor lấy, destructor trả. Smart pointer là RAII cho bộ nhớ heap (`#include <memory>`).
- `unique_ptr<T>`: sở hữu duy nhất. **Không copy được**, chỉ move (`std::move`). Kích thước bằng con trỏ thô. Tạo bằng `make_unique<T>(args)` (C++14). `get()` lấy con trỏ thô, `release()` bỏ quyền sở hữu (trả về con trỏ thô, không delete), `reset()` delete object cũ. `unique_ptr<T[]>` cho mảng.
- `shared_ptr<T>`: sở hữu chung qua **reference count** trong control block; `use_count()`. Object bị delete khi shared_ptr cuối cùng bị huỷ. Copy thì count tăng. `make_shared` cấp phát object + control block trong một lần. Kích thước = 2 con trỏ.
- `weak_ptr<T>`: không sở hữu, không tăng count. Dùng `lock()` để lấy shared_ptr (rỗng nếu object đã chết), `expired()`. Dùng để **phá vòng tham chiếu** (A ↔ B cùng giữ shared_ptr → không bao giờ giải phóng).
- **Custom deleter**: `unique_ptr<FILE, decltype(&fclose)> f{fopen(...), &fclose};` hoặc dùng lambda.
- Quy tắc: ưu tiên `unique_ptr`; chỉ dùng `shared_ptr` khi thực sự sở hữu chung. Không tạo hai smart pointer từ cùng một con trỏ thô. Truyền vào hàm chỉ để dùng thì truyền `T&` hoặc `T*`, không truyền smart pointer.

### Exception (section 18)
- `throw` ném một object (bất kỳ kiểu nào: int, string, class); `try { } catch (const T &e) { }` bắt.
- Khi throw, **stack unwinding**: các hàm bị thoát dần, destructor của mọi object local được gọi, tới khi gặp catch phù hợp. Không có catch nào khớp → `std::terminate()` → abort.
- catch được thử **theo thứ tự viết**, chọn cái đầu tiên khớp → đặt lớp con trước lớp cha. Không có chuyển đổi kiểu số (throw 3.14 không vào `catch (int)`), chỉ có chuyển đổi Derived → Base. `catch (...)` bắt mọi thứ, đặt cuối cùng.
- **Ném theo giá trị, bắt theo const reference** (tránh copy, tránh slicing, giữ đa hình của `what()`).
- `throw;` (không biểu thức) ném lại exception đang xử lý, giữ nguyên kiểu thật. `throw e;` ném bản copy (có thể bị slicing).
- Hệ thống lớp chuẩn (`<stdexcept>`): `std::exception` (`virtual const char *what() const noexcept`) → `logic_error` (`invalid_argument`, `out_of_range`, `length_error`, `domain_error`), `runtime_error` (`overflow_error`, `underflow_error`, `range_error`), `bad_alloc` (new thất bại), `bad_cast` (dynamic_cast reference thất bại).
- Lớp exception tự định nghĩa: kế thừa `std::exception` (override `what()`) hoặc `std::runtime_error` (truyền thông điệp vào constructor).
- `noexcept`: hàm hứa không ném. Nếu vẫn ném → `std::terminate`. Destructor mặc định là noexcept. **Không ném exception từ destructor.** Move constructor nên là noexcept để vector dùng move khi cấp phát lại.
- Exception trong constructor: object coi như chưa được tạo, destructor của nó **không** chạy (nhưng destructor của các member đã dựng xong vẫn chạy) → dùng RAII cho member.
- Không phải exception: chia số nguyên cho 0 (UB, thường SIGFPE), truy cập ngoài mảng bằng `[]`, dereference nullptr (SIGSEGV).
- Dùng exception cho trường hợp bất thường, không dùng làm luồng điều khiển thông thường.

### I/O và stream (section 19)
- Phân cấp: `ios_base` → `ios` → `istream` / `ostream` → `iostream`; file: `ifstream`, `ofstream`, `fstream` (`<fstream>`); chuỗi: `istringstream`, `ostringstream`, `stringstream` (`<sstream>`).
- Đối tượng chuẩn: `cin` (istream), `cout` (ostream, có buffer), `cerr` (không buffer), `clog` (có buffer).
- Manipulator (`<iomanip>`): `setw(n)` (**chỉ áp dụng cho lần in kế tiếp**), `setfill(c)`, `left`/`right`/`internal`, `fixed`/`scientific`, `setprecision(n)`, `showpoint`, `showpos`, `boolalpha`, `hex`/`dec`/`oct`, `showbase`, `uppercase`. Trừ setw, các manipulator đều **dính** tới khi bị đổi.
- `setprecision(n)` không có fixed: n **chữ số có nghĩa**; có fixed: n chữ số sau dấu phẩy. Mặc định 6 chữ số có nghĩa.
- Mở file: `std::ifstream in{"data.txt"};` hoặc `in.open(...)`. Luôn kiểm tra `if (!in)` hoặc `in.is_open()`. Chế độ: `ios::in`, `ios::out` (ghi đè), `ios::app` (ghi thêm vào cuối), `ios::trunc`, `ios::binary`, `ios::ate`. File tự đóng khi object bị huỷ (RAII); `close()` để đóng sớm.
- Đọc: `in >> x` (theo từ), `getline(in, line)` (theo dòng), `in.get(c)` (từng ký tự, kể cả khoảng trắng). Mẫu đúng: `while (getline(in, line)) {...}` hoặc `while (in >> x)`. **Không** dùng `while (!in.eof())` vì vòng cuối sẽ chạy thừa với dữ liệu cũ.
- Ghi: `out << x`, `out.put(c)`. Binary: `write(reinterpret_cast<const char*>(&x), sizeof x)` / `read`.
- Trạng thái stream: `good()`, `eof()`, `fail()` (đọc sai định dạng), `bad()` (lỗi nghiêm trọng). Khi fail, mọi thao tác đọc sau đều bị bỏ qua cho tới khi `clear()`; thường kèm `ignore(numeric_limits<streamsize>::max(), '\n')` để bỏ phần input hỏng.
- `stringstream` dùng để tách chuỗi, chuyển chuỗi ↔ số, kiểm tra input hợp lệ (`if (ss >> n && ss.eof())`).

### STL (section 20)
- Ba thành phần: **container** (chứa dữ liệu), **iterator** (duyệt container, như con trỏ tổng quát), **algorithm** (`<algorithm>`, `<numeric>`, làm việc trên khoảng iterator `[first, last)`). Thêm functor/lambda làm tiêu chí.
- Template: `template <typename T> T max(T a, T b)`, class template `template <typename T> class Item`. Code được sinh khi dùng → định nghĩa phải đặt trong header. Non-type parameter: `template <int N>`.
- `begin()` trỏ phần tử đầu, `end()` trỏ **sau** phần tử cuối (không dereference được). `cbegin`/`cend` (const), `rbegin`/`rend` (ngược).
- Loại iterator: input, output, forward (`forward_list`, `unordered_*`), bidirectional (`list`, `set`, `map`), random access (`vector`, `deque`, `array`, `string`). `std::sort` cần random access → `list` dùng `l.sort()`.
- Sequence container: `array` (cố định, không cấp phát động), `vector` (thêm/xoá ở cuối nhanh), `deque` (thêm/xoá hai đầu nhanh, không liền nhau hoàn toàn), `list` (liên kết đôi, chèn giữa O(1) khi có iterator, không có `[]`), `forward_list` (liên kết đơn, không có `size()`).
- Associative: `set` (key duy nhất, có thứ tự), `multiset` (cho trùng), `map` (key → value, có thứ tự), `multimap`. Unordered (hash, `unordered_set`, `unordered_map`): trung bình O(1), không có thứ tự.
- `map[key]` tự chèn phần tử mặc định nếu chưa có; `at(key)` ném `out_of_range`; `find`/`count` để kiểm tra. `insert` không ghi đè key đã có, trả về `pair<iterator, bool>`.
- Adapter (không có iterator): `stack` (LIFO: `push`/`pop`/`top`), `queue` (FIFO: `push`/`pop`/`front`/`back`), `priority_queue` (heap, mặc định **max-heap**: `top` là phần tử lớn nhất; min-heap thì dùng `priority_queue<int, vector<int>, greater<int>>`). `pop()` không trả về giá trị.
- Thuật toán hay dùng: `find`, `find_if`, `count`, `count_if`, `sort`, `stable_sort`, `binary_search` (cần đã sắp xếp), `min_element`, `max_element`, `for_each`, `transform`, `copy`, `replace`, `reverse`, `unique` (chỉ bỏ trùng **liền kề**), `remove`/`remove_if` (không đổi size → cần `erase`), `accumulate` (kiểu kết quả lấy theo giá trị khởi đầu: 0 → int).
- Iterator invalidation: vector cấp phát lại làm mọi iterator vô hiệu; xoá trong vòng lặp dùng `it = c.erase(it)`.

### Lambda (section 21)
- `[capture](params) mutable noexcept -> ret { body }`. Kiểu trả về thường tự suy ra.
- Trước lambda: function object (functor), tức class có `operator()`. Lambda thực chất được compiler biến thành một functor ẩn danh; mỗi lambda có kiểu riêng → lưu bằng `auto` hoặc `std::function<R(Args...)>`.
- **Stateless** (`[]`): không capture, chuyển được thành con trỏ hàm. **Stateful**: có capture.
- `[x]` copy lúc **tạo** lambda (chỉ đọc, trừ khi có `mutable`); `[&x]` tham chiếu, đọc giá trị lúc **gọi**; `[=]` copy mọi biến được dùng; `[&]` tham chiếu mọi biến; trộn `[=, &total]`, `[&, x]`; `[this]` (capture con trỏ this), `[*this]` (C++17); init capture `[y = x + 1]`, `[p = std::move(ptr)]` (C++14).
- `mutable`: cho sửa bản copy riêng của lambda; bản copy giữ trạng thái giữa các lần gọi, biến bên ngoài không đổi.
- Generic lambda (C++14): `[](auto a, auto b) { return a < b; }`.
- Dùng nhiều nhất làm predicate/comparator cho thuật toán STL: `sort(v.begin(), v.end(), [](const P &a, const P &b) { return a.age < b.age; });`.
- Nguy hiểm: capture theo reference rồi dùng lambda sau khi biến hết scope (trả lambda `[&]` ra khỏi hàm, lưu vào thread) → dangling.

### Enum (section 23)
- **Unscoped** `enum Color { Red, Green = 5, Blue };`: tên tràn ra phạm vi bao ngoài (trùng tên giữa các enum → lỗi), tự chuyển ngầm sang int. Giá trị mặc định bắt đầu từ 0, phần tử không gán thì bằng phần tử trước + 1 (Blue = 6).
- **Scoped** `enum class Color { Red, Green };` (C++11): phải viết `Color::Red`, **không** chuyển ngầm sang int (cần `static_cast<int>`), không so sánh được với enum khác → an toàn kiểu.
- Kiểu nền (underlying type): `enum class Status : char { ... };` → `sizeof` = 1. Mặc định của enum class là int.
- Dùng tốt với `switch` (gcc cảnh báo `-Wswitch` khi thiếu case).

## Trắc nghiệm

### Đoạn code nào biên dịch được?
```cpp
auto p1 = std::make_unique<int>(5);
```
- [ ] `auto p2 = p1;`
- [x] `auto p2 = std::move(p1);`
- [ ] `std::unique_ptr<int> p2(p1);`
- [ ] `std::unique_ptr<int> p2 = p1.get();`
> unique_ptr không copy được, chỉ move. Sau khi move, p1 trở thành nullptr. Không có chuyển đổi ngầm từ con trỏ thô sang unique_ptr.

### `use_count()` in ra gì?
```cpp
auto a = std::make_shared<int>(1);
{
    auto b = a;
    auto c = b;
}
std::cout << a.use_count();
```
- [ ] 3
- [x] 1
- [ ] 0
- [ ] 2
> b và c tăng count lên 3 nhưng bị huỷ khi ra khỏi khối, nên count còn 1.

### Mục đích chính của `weak_ptr`?
- [ ] Nhanh hơn shared_ptr
- [x] Quan sát object do shared_ptr quản lý mà không tăng reference count, dùng để phá vòng tham chiếu
- [ ] Thay thế unique_ptr
- [ ] Quản lý mảng
> Truy cập object qua `lock()`; nếu object đã bị huỷ thì `lock()` trả về shared_ptr rỗng.

### A giữ `shared_ptr<B>`, B giữ `shared_ptr<A>` trỏ lẫn nhau. Khi các shared_ptr bên ngoài bị huỷ thì sao?
- [ ] Cả hai được giải phóng bình thường
- [x] Không object nào được giải phóng (memory leak) vì count không về 0
- [ ] Double free
- [ ] Lỗi biên dịch
> Cách sửa: một phía giữ `weak_ptr`.

### Vì sao nên dùng `make_shared<T>()` thay cho `shared_ptr<T>(new T)`?
- [ ] Vì `new` bị cấm trong C++11
- [x] Cấp phát object và control block trong một lần (nhanh hơn, ít phân mảnh) và an toàn khi có exception
- [ ] Vì shared_ptr(new T) không đếm reference
- [ ] Vì make_shared trả về unique_ptr
> Nhược điểm nhỏ: khi còn weak_ptr thì vùng nhớ của object chưa được trả lại, vì nằm chung khối với control block.

### RAII nghĩa là gì?
- [ ] Mọi biến phải được khởi tạo
- [x] Gắn tài nguyên (bộ nhớ, file, mutex) với vòng đời object: lấy trong constructor, trả trong destructor
- [ ] Chỉ cấp phát trên stack
- [ ] Chỉ dùng smart pointer
> Destructor luôn chạy khi object hết scope, kể cả khi có exception (stack unwinding). Ví dụ: smart pointer, `std::lock_guard`, `std::fstream`, `std::vector`.

### Đoạn code in ra gì?
```cpp
try { throw 3.14; }
catch (int e)   { cout << "int"; }
catch (float e) { cout << "float"; }
catch (...)     { cout << "other"; }
```
- [ ] int
- [ ] float
- [x] other
- [ ] Lỗi biên dịch
> 3.14 là double. Catch không chuyển đổi kiểu số (double → int, double → float), chỉ chuyển đổi Derived → Base.

### Thứ tự catch nào ĐÚNG?
- [ ] `catch (const std::exception&)` rồi `catch (const std::out_of_range&)`
- [x] `catch (const std::out_of_range&)` rồi `catch (const std::exception&)`
- [ ] Thứ tự không quan trọng, compiler chọn cái khớp nhất
- [ ] `catch (...)` đặt đầu tiên
> Catch chọn khối khớp **đầu tiên** theo thứ tự viết. Lớp cha đặt trước sẽ che lớp con (gcc cảnh báo). `catch (...)` phải đặt cuối.

### Vì sao nên bắt exception bằng `const T&`?
- [ ] Bắt buộc theo chuẩn
- [x] Tránh copy và tránh slicing (giữ được what() của lớp con)
- [ ] Để sửa được exception
- [ ] Để exception không bị huỷ
> `catch (std::exception e)` copy phần base → what() trả về thông điệp của std::exception.

### Khác nhau giữa `throw;` và `throw e;` trong khối catch?
- [ ] Không khác
- [x] `throw;` ném lại đúng object gốc (giữ kiểu thật); `throw e;` ném bản copy theo kiểu tĩnh của e (có thể bị slicing)
- [ ] `throw;` không hợp lệ
- [ ] `throw e;` kết thúc chương trình
> Ở ngoài catch mà gọi `throw;` (không có exception nào đang được xử lý) thì gọi std::terminate.

### Exception được ném ra nhưng không có catch nào bắt thì sao?
- [ ] Chương trình bỏ qua và chạy tiếp
- [x] `std::terminate()` được gọi, mặc định gọi abort
- [ ] Trả về mã lỗi -1
- [ ] Lỗi biên dịch
> Khi terminate, việc stack unwinding có xảy ra hay không là do implementation quyết định.

### Điều gì xảy ra khi exception thoát ra khỏi một hàm `noexcept`?
- [ ] Bị bỏ qua
- [ ] Truyền lên hàm gọi như bình thường
- [x] `std::terminate()` được gọi
- [ ] Lỗi biên dịch
> noexcept là lời hứa. Compiler không kiểm tra lúc biên dịch, nhưng lúc chạy thì terminate.

### Vì sao không nên ném exception từ destructor?
- [ ] Destructor không thể chứa try
- [x] Destructor mặc định là noexcept, và nếu đang stack unwinding mà lại có exception thứ hai thì std::terminate
- [ ] Vì destructor không có giá trị trả về
- [ ] Chuẩn cấm hoàn toàn
> Nếu destructor có thể lỗi, hãy xử lý (log) ngay bên trong hoặc cung cấp hàm `close()` riêng để người dùng gọi.

### Constructor ném exception giữa chừng. Điều nào ĐÚNG?
- [ ] Destructor của object vẫn được gọi
- [x] Destructor của object không được gọi, nhưng destructor của các member đã dựng xong vẫn chạy
- [ ] Bộ nhớ của object bị leak
- [ ] Không được ném exception trong constructor
> Vì vậy member nên là RAII (unique_ptr, vector) thay vì con trỏ thô cấp phát trong thân constructor.

### `int x = 10 / 0;` (với số 0 lúc chạy) có bị `catch (...)` bắt không?
- [ ] Có, ném std::runtime_error
- [ ] Có, ném std::domain_error
- [x] Không, đây là undefined behavior (trên Linux thường là tín hiệu SIGFPE)
- [ ] Không, vì lỗi biên dịch
> C++ không tự ném exception cho lỗi phần cứng. Phải tự kiểm tra rồi throw.

### `std::stoi("abc")` ném exception gì?
- [ ] `std::out_of_range`
- [x] `std::invalid_argument`
- [ ] `std::bad_cast`
- [ ] Không ném, trả về 0
> Số quá lớn so với int (`stoi("99999999999")`) thì ném `std::out_of_range`.

### `new` thất bại vì hết bộ nhớ thì mặc định làm gì?
- [ ] Trả về nullptr
- [x] Ném `std::bad_alloc`
- [ ] Gọi abort ngay lập tức
- [ ] Trả về con trỏ rác
> `new (std::nothrow) T` mới trả về nullptr. malloc thì trả về NULL.

### Đoạn code in ra gì?
```cpp
cout << '[' << setw(5) << 42 << "][" << 42 << ']';
```
- [ ] `[   42][   42]`
- [x] `[   42][42]`
- [ ] `[42   ][42]`
- [ ] `[42][42]`
> setw chỉ áp dụng cho lần in kế tiếp, mặc định canh phải.

### `cout << setprecision(3) << 3.14159;` (không có fixed) in ra?
- [ ] 3.142
- [x] 3.14
- [ ] 3.141
- [ ] 3.1
> Không có fixed: precision là số chữ số có nghĩa (3 chữ số). Với `fixed` thì là 3 chữ số sau dấu phẩy: 3.142.

### `cout << hex << 255 << ' ' << 16;` in ra?
- [ ] ff 16
- [x] ff 10
- [ ] 0xff 0x10
- [ ] 255 16
> hex là manipulator "dính", có hiệu lực tới khi gặp `dec`. Muốn có tiền tố 0x thì thêm `showbase`.

### Vì sao `while (!file.eof()) { file >> x; process(x); }` là cách đọc sai?
- [ ] eof() không tồn tại
- [x] eof chỉ bật sau khi một lần đọc thất bại, nên vòng cuối xử lý lại giá trị cũ (hoặc giá trị lỗi)
- [ ] Đọc thiếu phần tử đầu tiên
- [ ] Vòng lặp vô hạn trong mọi trường hợp
> Đúng: `while (file >> x) { process(x); }`, kiểm tra kết quả của chính lần đọc.

### Chế độ mở file nào để ghi thêm vào cuối, không xoá nội dung cũ?
- [ ] `ios::out`
- [ ] `ios::trunc`
- [x] `ios::app`
- [ ] `ios::in`
> `ofstream` mặc định là `ios::out | ios::trunc` (xoá nội dung cũ).

### Đọc chuỗi "12 abc 34" vào ba biến int bằng `ss >> a >> b >> c;` thì?
- [ ] a = 12, b = 0, c = 34
- [x] a = 12, b = 0, c không được đọc (giữ giá trị cũ), stream ở trạng thái fail
- [ ] Ném exception
- [ ] a = 12, b = 34
> Sau khi fail, mọi lần đọc tiếp đều bị bỏ qua cho tới khi gọi `clear()`. C++11 gán 0 cho biến đọc thất bại.

### `end()` của một container trỏ tới đâu?
- [ ] Phần tử cuối cùng
- [x] Vị trí ngay sau phần tử cuối cùng (không được dereference)
- [ ] nullptr
- [ ] Phần tử đầu tiên
> Khoảng `[begin, end)` là nửa mở; container rỗng thì `begin() == end()`.

### Sau đoạn code, `v.size()` bằng?
```cpp
vector<int> v{1, 2, 3, 2, 2};
remove(v.begin(), v.end(), 2);
```
- [ ] 2
- [x] 5
- [ ] 3
- [ ] 0
> remove chỉ dồn phần tử cần giữ lên đầu và trả về iterator "end mới"; không đổi size. Phải dùng `v.erase(remove(...), v.end());`.

### `accumulate(v.begin(), v.end(), 0)` với `vector<double> v{0.5, 0.5, 0.5}` bằng?
- [ ] 1.5
- [x] 0
- [ ] 1
- [ ] Lỗi biên dịch
> Kiểu tổng lấy theo giá trị khởi đầu (0 → int): 0 + 0.5 → 0, cứ thế, kết quả 0. Phải dùng `0.0`.

### `set<int> s{3, 1, 3, 2, 1};` duyệt in ra?
- [ ] 3 1 3 2 1
- [ ] 3 1 2
- [x] 1 2 3
- [ ] 1 1 2 3 3
> set loại phần tử trùng và sắp xếp tăng dần (cây đỏ-đen). multiset thì giữ trùng.

### `priority_queue<int>` lần lượt push 3, 1, 4. `top()` bằng?
- [ ] 3
- [ ] 1
- [x] 4
- [ ] Lỗi vì chưa sort
> Mặc định là max-heap. Min-heap: `priority_queue<int, vector<int>, greater<int>>`.

### Container adapter nào là LIFO?
- [x] `stack`
- [ ] `queue`
- [ ] `deque`
- [ ] `priority_queue`
> `queue` là FIFO. `deque` là container thật (có iterator), không phải adapter; stack và queue mặc định dùng deque bên dưới.

### Vì sao không gọi `std::sort` trên `std::list` được?
- [ ] list không có phần tử
- [x] std::sort cần random access iterator, list chỉ có bidirectional
- [ ] list luôn được sắp xếp sẵn
- [ ] sort chỉ dùng cho mảng C
> Dùng hàm thành viên `l.sort()` (merge sort trên danh sách liên kết).

### Đoạn code in ra gì?
```cpp
int x = 1;
auto f = [x]() { return x; };
auto g = [&x]() { return x; };
x = 10;
cout << f() << ' ' << g();
```
- [ ] 10 10
- [x] 1 10
- [ ] 1 1
- [ ] 10 1
> Capture theo giá trị chụp lúc tạo lambda; capture theo tham chiếu đọc giá trị lúc gọi.

### Lambda `[n]() { return ++n; }` (không có mutable) thì sao?
- [ ] Trả về n + 1, n ngoài tăng
- [ ] Trả về n + 1, n ngoài không đổi
- [x] Lỗi biên dịch: biến capture theo giá trị là read-only
- [ ] Undefined behavior
> `operator()` của lambda mặc định là const. Thêm `mutable` để sửa bản copy riêng; biến ngoài vẫn không đổi.

### Lambda nào chuyển được thành con trỏ hàm `int (*)(int)`?
- [x] `[](int a) { return a * 2; }`
- [ ] `[k](int a) { return a * k; }`
- [ ] `[&](int a) { return a; }`
- [ ] `[=](int a) { return a; }` có dùng biến ngoài
> Chỉ lambda không capture (stateless) mới chuyển được thành con trỏ hàm, ví dụ để truyền cho `qsort` hoặc API C.

### Hàm trả về `[&local]() { return local; }` với `local` là biến local của hàm. Gọi lambda sau khi hàm kết thúc thì sao?
- [ ] Trả về giá trị cuối của local
- [x] Undefined behavior (dangling reference)
- [ ] Lỗi biên dịch
- [ ] Trả về 0
> Khi lambda sống lâu hơn scope hiện tại thì capture theo giá trị (`[local]`) hoặc move vào (`[p = std::move(p)]`).

### `enum Color { Red, Green = 5, Blue };` thì `Blue` bằng?
- [ ] 2
- [x] 6
- [ ] 7
- [ ] 0
> Phần tử không gán giá trị bằng phần tử trước + 1. Red = 0.

### Điều nào ĐÚNG về `enum class` so với enum thường? (chọn tất cả đáp án đúng)
- [x] Phải viết kèm tên: `Color::Red`
- [x] Không tự chuyển sang int, phải dùng static_cast
- [x] Hai enum class khác nhau có thể dùng chung tên phần tử (Red) mà không xung đột
- [ ] Không dùng được trong switch
> enum class dùng tốt trong switch với `case Color::Red:`.

### `enum class Status : char { Ok, Error };` thì `sizeof(Status)` bằng?
- [x] 1
- [ ] 4
- [ ] 8
- [ ] 2
> Kiểu nền được chỉ định là char. Không chỉ định thì enum class mặc định dùng int (4 byte).
