# C++ hướng đối tượng

## Lý thuyết

### Bốn tính chất OOP
- **Encapsulation** (đóng gói): giấu dữ liệu bằng `private`/`protected`, lộ ra qua hàm public.
- **Inheritance** (kế thừa): `public` (is-a), `protected`, `private` (implemented-in-terms-of). struct mặc định public, class mặc định private (cả member lẫn kiểu kế thừa).
- **Polymorphism** (đa hình): compile-time (overload, template) và run-time (hàm `virtual`).
- **Abstraction** (trừu tượng): lớp trừu tượng có ít nhất một pure virtual `virtual void f() = 0;` — không tạo object được.

### Constructor / destructor
- Thứ tự khởi tạo: virtual base → base (theo thứ tự khai báo kế thừa) → member (theo thứ tự **khai báo** trong class, không theo initializer list) → thân constructor. Destructor theo thứ tự ngược lại.
- Member `const`, reference, và member không có default constructor **bắt buộc** khởi tạo trong initializer list.
- Constructor một tham số có thể chuyển kiểu ngầm → dùng `explicit` để chặn.
- Gọi hàm virtual trong constructor/destructor → chạy bản của lớp **đang** được dựng/huỷ, không phải lớp con.
- Lớp cơ sở dùng đa hình phải có **destructor virtual**, nếu không `delete base_ptr` sẽ không gọi destructor lớp con (UB).

### Các hàm đặc biệt (special member functions)
Default ctor, destructor, copy ctor `T(const T&)`, copy assignment `T& operator=(const T&)`, move ctor `T(T&&)`, move assignment `T& operator=(T&&)`.
- **Rule of three**: tự viết một trong destructor/copy ctor/copy assign thì thường phải viết cả ba (lớp quản lý tài nguyên).
- **Rule of five**: thêm move ctor và move assign. **Rule of zero**: dùng RAII (smart pointer, container) để không phải viết cái nào.
- Shallow copy (copy con trỏ) → hai object cùng free một vùng nhớ → double free. Deep copy cấp phát vùng mới.
- Copy ctor được gọi khi: khởi tạo từ object khác, truyền tham số theo giá trị, trả về theo giá trị (nếu không có elision). Copy assignment: gán cho object **đã tồn tại**.
- `= default`, `= delete` (ví dụ cấm copy: `T(const T&) = delete;`).

### Virtual và vtable
- Mỗi class có hàm virtual có một **vtable** (bảng con trỏ hàm), mỗi object có một **vptr** trỏ tới vtable của class nó.
- Dynamic binding chỉ xảy ra khi gọi qua **con trỏ hoặc reference**. Gọi qua object (by value) → static binding; gán object con cho object cha → **object slicing**.
- `override` (kiểm tra có thật sự ghi đè), `final` (cấm ghi đè/kế thừa tiếp).
- Tham số mặc định của hàm virtual lấy theo kiểu tĩnh.

### Overload, override, hide
- **Overload**: cùng tên, khác tham số, cùng phạm vi; quyết định lúc biên dịch. Không overload được chỉ bằng kiểu trả về.
- **Override**: lớp con định nghĩa lại hàm virtual cùng chữ ký.
- **Hide**: lớp con khai báo hàm cùng tên (khác tham số, hoặc base không virtual) → che **mọi** overload cùng tên của base. Dùng `using Base::f;` để kéo lại.

### Các từ khoá khác
- `static` member: dùng chung cho mọi object; static member function không có `this`.
- `const` member function: không sửa member (trừ `mutable`); object const chỉ gọi được hàm const.
- `friend`: cho hàm/lớp khác truy cập private; không kế thừa, không bắc cầu.
- `inline`, `namespace`, reference (`int &r = x;` phải khởi tạo, không đổi được nơi tham chiếu, không có null reference).
- Cast: `static_cast` (chuyển kiểu có kiểm tra lúc biên dịch), `dynamic_cast` (downcast an toàn lúc chạy, cần class đa hình, trả nullptr/ném `bad_cast`), `const_cast` (bỏ const), `reinterpret_cast` (diễn giải lại bit).
- `new`/`delete` gọi constructor/destructor; `malloc`/`free` thì không. `new[]` phải đi với `delete[]`.

### Exception
- `throw` → stack unwinding huỷ các object local → khớp `catch` đầu tiên phù hợp. Bắt bằng `const T&`.
- Destructor mặc định `noexcept`; ném exception lúc đang unwinding → `std::terminate`.
- RAII: tài nguyên gắn với vòng đời object (lock_guard, unique_ptr, fstream).

## Trắc nghiệm

### Thứ tự khởi tạo member trong constructor được quyết định bởi?
- [ ] Thứ tự viết trong initializer list
- [x] Thứ tự khai báo member trong class
- [ ] Thứ tự bảng chữ cái của tên member
- [ ] Do compiler tự chọn
> Luôn theo thứ tự khai báo. Viết initializer list khác thứ tự sẽ bị cảnh báo -Wreorder và có thể dùng member chưa khởi tạo.

### Đoạn code in ra gì?
```cpp
struct B { B(){ cout << "B"; } ~B(){ cout << "~B"; } };
struct D : B { D(){ cout << "D"; } ~D(){ cout << "~D"; } };
int main(){ D d; }
```
- [ ] D B ~B ~D
- [x] B D ~D ~B
- [ ] B D ~B ~D
- [ ] D B ~D ~B
> Base được dựng trước, huỷ sau.

### Điều gì xảy ra khi `delete` qua con trỏ Base tới object Derived, mà destructor của Base không virtual?
```cpp
Base *p = new Derived; delete p;
```
- [ ] Gọi ~Derived rồi ~Base
- [ ] Chỉ gọi ~Derived
- [x] Undefined behavior; thực tế thường chỉ gọi ~Base nên rò rỉ tài nguyên của Derived
- [ ] Lỗi biên dịch
> Lớp cơ sở dùng đa hình phải có destructor virtual.

### Gọi hàm virtual bên trong constructor của Base (đang dựng object Derived) sẽ chạy bản nào?
- [x] Bản của Base
- [ ] Bản của Derived
- [ ] Undefined behavior luôn crash
- [ ] Lỗi biên dịch
> Trong lúc Base đang được dựng, phần Derived chưa tồn tại; vptr đang trỏ vtable của Base.

### Object slicing xảy ra khi nào?
- [ ] Khi gán con trỏ Derived* cho Base*
- [x] Khi copy object Derived vào biến kiểu Base (theo giá trị)
- [ ] Khi dùng dynamic_cast
- [ ] Khi gọi hàm virtual qua reference
> Chỉ phần Base được copy; phần riêng của Derived bị cắt mất và gọi hàm virtual sẽ chạy bản của Base.

### Dynamic binding (gọi đúng hàm virtual của lớp con) chỉ xảy ra khi gọi qua?
- [ ] Object theo giá trị
- [x] Con trỏ hoặc reference tới lớp cơ sở
- [ ] Tên lớp kèm `::`
- [ ] Hàm static
> Gọi qua object hoặc dùng `Base::f()` tường minh đều là static binding.

### Mỗi object của một class có hàm virtual chứa thêm gì?
- [ ] Một bản copy của vtable
- [x] Một con trỏ vptr tới vtable dùng chung của class
- [ ] Con trỏ tới từng hàm virtual
- [ ] Không chứa gì thêm
> vtable có một bản cho mỗi class; mỗi object chỉ thêm một vptr (8 byte trên 64-bit), bất kể có bao nhiêu hàm virtual.

### Khai báo nào làm class trở thành abstract?
- [ ] `virtual void f();`
- [x] `virtual void f() = 0;`
- [ ] `void f() const;`
- [ ] `static void f();`
> Pure virtual function. Lớp con phải override thì mới tạo object được.

### Copy constructor được gọi trong trường hợp nào? (chọn tất cả đáp án đúng)
- [x] `T b = a;`
- [x] Truyền object theo giá trị vào hàm `void f(T x)`
- [ ] `b = a;` khi b đã tồn tại
- [x] `T b(a);`
> `b = a` với b đã tồn tại gọi copy assignment operator, không phải copy constructor.

### Vì sao tham số của copy constructor phải là reference `T(const T&)`?
- [ ] Để chạy nhanh hơn
- [x] Nếu truyền theo giá trị, việc truyền tham số lại cần gọi copy constructor → đệ quy vô hạn
- [ ] Vì chuẩn cấm const
- [ ] Để có thể sửa object nguồn
> Compiler báo lỗi nếu viết `T(T other)`.

### Rule of three nói rằng nếu tự viết một trong các hàm sau thì thường phải viết cả ba:
- [ ] constructor, destructor, operator==
- [x] destructor, copy constructor, copy assignment
- [ ] move constructor, move assignment, destructor
- [ ] constructor, copy constructor, operator<<
> Cả ba liên quan tới quản lý tài nguyên. Rule of five thêm move constructor và move assignment.

### Hàm `void f(double)` ở Derived ảnh hưởng gì tới `void f(int)` của Base?
```cpp
struct Base { void f(int); };
struct Derived : Base { void f(double); };
Derived d; d.f(1);
```
- [ ] Gọi Base::f(int) vì khớp chính xác hơn
- [x] Gọi Derived::f(double) vì Derived::f che mọi f của Base
- [ ] Mơ hồ, lỗi biên dịch
- [ ] Lỗi liên kết
> Name hiding. Thêm `using Base::f;` trong Derived để overload cả hai.

### Có thể overload hai hàm chỉ khác nhau ở kiểu trả về không?
- [ ] Có
- [x] Không
- [ ] Chỉ khi một hàm là virtual
- [ ] Chỉ trong C++17
> Overload resolution chỉ dựa vào danh sách tham số (và cv/ref qualifier của hàm thành viên).

### `dynamic_cast<Derived*>(basePtr)` trả về gì khi basePtr không thực sự trỏ tới Derived?
- [ ] Ném exception std::bad_cast
- [x] nullptr
- [ ] Con trỏ không hợp lệ (UB)
- [ ] Lỗi biên dịch
> Với con trỏ trả nullptr; với reference ném `std::bad_cast`. dynamic_cast cần class có ít nhất một hàm virtual.

### Member nào BẮT BUỘC phải khởi tạo trong initializer list? (chọn tất cả đáp án đúng)
- [x] Member `const int`
- [x] Member reference `int &`
- [ ] Member `static int`
- [x] Member là object của class không có default constructor
> Static member được định nghĩa ngoài class, không thuộc object.

### Từ khoá `explicit` trên constructor dùng để?
- [ ] Bắt buộc gọi constructor bằng `new`
- [x] Chặn chuyển kiểu ngầm định qua constructor đó
- [ ] Cho phép constructor ném exception
- [ ] Làm constructor thành inline
> Ví dụ `explicit String(int n)` cấm viết `String s = 10;`.

### `mutable` cho phép điều gì?
- [ ] Sửa biến const toàn cục
- [x] Sửa member đó trong hàm thành viên const
- [ ] Đổi kiểu biến lúc chạy
- [ ] Bỏ qua kiểm tra truy cập private
> Hay dùng cho cache, mutex bên trong object được truy cập qua hàm const.

### Điểm khác nhau giữa `new` và `malloc`? (chọn tất cả đáp án đúng)
- [x] new gọi constructor, malloc thì không
- [x] new ném std::bad_alloc khi thất bại, malloc trả NULL
- [ ] malloc trả về đúng kiểu, không cần ép
- [x] new không cần truyền số byte
> Trong C++, malloc trả `void*` và phải ép kiểu. new[] phải đi với delete[].

### Kế thừa `class D : B` (không ghi gì) là kiểu kế thừa nào?
- [ ] public
- [ ] protected
- [x] private
- [ ] virtual
> Với `class` mặc định private; với `struct` mặc định public.

### Đoạn code in ra gì?
```cpp
struct A { virtual void f(int x = 1) { cout << "A" << x; } };
struct B : A { void f(int x = 2) override { cout << "B" << x; } };
A *p = new B; p->f();
```
- [ ] A1
- [x] B1
- [ ] B2
- [ ] A2
> Hàm được chọn lúc chạy (B::f), nhưng tham số mặc định lấy theo kiểu tĩnh A → 1.

### Diamond problem được giải quyết bằng cách nào để chỉ có một bản của lớp cơ sở chung?
- [ ] Dùng `final`
- [x] Kế thừa `virtual`
- [ ] Dùng `friend`
- [ ] Dùng template
> `struct B : virtual A`, `struct C : virtual A`, `struct D : B, C` → D chỉ chứa một A, và D gọi constructor của A trực tiếp.
