# Class, operator overloading, kế thừa, đa hình

## Lý thuyết

Theo Udemy *Beginning C++ Programming*, section 13–16. Lab: `02-cpp/23` … `02-cpp/25`, xem thêm `02-cpp/01` … `05`, `09`, `10`, `12`, `13`. Chủ đề "C++ OOP" ở trên có phần tóm tắt ngắn hơn.

### Class và object (section 13)
- Class là bản thiết kế (kiểu do người dùng định nghĩa); object là một thể hiện (instance) của class.
- Access specifier: `public` (mọi nơi), `private` (chỉ trong class và friend), `protected` (thêm lớp con). `class` mặc định private, `struct` mặc định public; ngoài ra hai từ khoá giống hệt nhau.
- **Encapsulation**: dữ liệu private, truy cập qua getter/setter để kiểm soát giá trị hợp lệ.
- Member function định nghĩa bên trong class thì ngầm là `inline`. Định nghĩa bên ngoài: `void Player::talk() {...}`. Thường khai báo class trong `.h` (kèm include guard `#ifndef/#define/#endif` hoặc `#pragma once`), định nghĩa trong `.cpp`.
- **Constructor**: cùng tên class, không kiểu trả về, overload được. **Destructor** `~T()`: không tham số, không overload, tự gọi khi object hết scope hoặc bị `delete`.
- **Default constructor** (không tham số) chỉ được compiler sinh khi class **không có constructor nào**. Đã viết `Player(string)` thì `Player p;` báo lỗi, trừ khi thêm `Player() = default;`.
- **Initializer list** `T() : a{1}, b{2} {}`: khởi tạo thẳng thay vì gán sau. Thứ tự khởi tạo theo **thứ tự khai báo member**, không theo thứ tự viết trong list. Bắt buộc dùng cho member `const`, reference, và member/lớp cha không có default constructor.
- **Delegating constructor** (C++11): `Player() : Player{"None", 0, 0} {}`. Constructor đích chạy xong (cả thân) rồi mới tới thân của constructor ủy quyền. Không dùng chung với việc khởi tạo member khác trong cùng list.
- Tham số mặc định trên constructor giảm số overload; cẩn thận ambiguous với default constructor.
- **Copy constructor** `T(const T &src)`: gọi khi khởi tạo từ object cùng kiểu (`T b = a;`, `T b(a);`), truyền theo giá trị, trả về theo giá trị (có thể bị copy elision bỏ qua). Tham số phải là reference, nếu không sẽ đệ quy vô hạn.
- **Shallow copy** (mặc định): copy từng member, con trỏ chỉ copy địa chỉ → hai object cùng trỏ một vùng → double free khi cả hai destructor delete. **Deep copy**: tự cấp phát vùng mới và copy dữ liệu.
- **Move constructor** `T(T &&src) noexcept`: "lấy" tài nguyên của object tạm (copy con trỏ rồi gán con trỏ nguồn = nullptr), không cấp phát mới. Gọi khi nguồn là rvalue hoặc `std::move(x)`.
- `this`: con trỏ tới object hiện tại. Dùng khi tên tham số trùng tên member (`this->name = name`), trả về `*this` để nối chuỗi, kiểm tra tự gán `if (this == &rhs)`.
- **const object** chỉ gọi được **const member function** `int get() const;`. Trong hàm const, `this` có kiểu `const T*`. `mutable` member vẫn sửa được.
- **static member**: dùng chung cho mọi object của class; phải định nghĩa một lần ngoài class (`int Player::count = 0;`) hoặc dùng `inline static` (C++17). **static member function** không có `this`, chỉ truy cập static member, gọi bằng `Player::get_count()`.
- **friend**: hàm/class bên ngoài được truy cập private. Quan hệ friend không đối xứng, không bắc cầu, không kế thừa.
- `struct` vs `class`: theo quy ước, struct cho dữ liệu đơn giản (public), class cho đối tượng có bất biến (invariant) và đóng gói.

### Operator overloading (section 14)
- Cú pháp member `T operator+(const T &rhs) const;` (vế trái là `*this`) hoặc non-member `T operator+(const T &lhs, const T &rhs);`.
- **Không** overload được: `::` `.` `.*` `?:` `sizeof` (và `typeid`, `alignof`). Không tạo toán tử mới, không đổi độ ưu tiên, tính kết hợp, hay số toán hạng. Phải có ít nhất một toán hạng là kiểu do người dùng định nghĩa.
- Bắt buộc là member: `=` `[]` `()` `->` và các toán tử chuyển kiểu.
- Phải là non-member khi vế trái không phải class của mình: `ostream &operator<<(ostream &os, const T &obj)` (trả về `os` để nối chuỗi), `T operator*(int, const T&)`. Thường khai báo là friend.
- **Copy assignment** `T &operator=(const T &rhs)`: kiểm tra tự gán, giải phóng tài nguyên cũ, deep copy, `return *this`. **Move assignment** `T &operator=(T &&rhs) noexcept`.
- Phân biệt: `T b = a;` gọi copy **constructor**; `b = a;` (b đã tồn tại) gọi copy **assignment**.
- Prefix `T &operator++()` trả về `*this`; postfix `T operator++(int)` (tham số int giả) trả về bản copy cũ.
- Unary `-`, `!` là member không tham số. So sánh `==`, `<` thường trả về `bool`. C++20: `operator<=>` sinh tự động các toán tử so sánh.
- Constructor một tham số không `explicit` là **chuyển đổi ngầm**: `Vec v = 5;`. Thêm `explicit` để chặn.

### Kế thừa (section 15)
- Quan hệ **is-a** (Savings is-a Account) → kế thừa. **has-a** (Player has-a Weapon) → composition (thành viên).
- `class Derived : public Base`. Kiểu kế thừa (mặc định `private` với class, `public` với struct):

| Member trong Base | public inheritance | protected inheritance | private inheritance |
| --- | --- | --- | --- |
| public | public | protected | private |
| protected | protected | protected | private |
| private | không truy cập được | không truy cập được | không truy cập được |

- Chỉ public inheritance là quan hệ is-a thật (con trỏ Base* trỏ được tới Derived).
- Lớp con **không kế thừa**: constructor, destructor, copy/move assignment, friend. C++11: `using Base::Base;` để dùng lại constructor của lớp cha.
- Thứ tự: constructor Base → member của Derived → thân constructor Derived. Destructor ngược lại. Gọi constructor Base có tham số: `Derived(int x) : Base{x}, ... {}`; không ghi thì gọi default constructor của Base.
- Copy constructor/operator= tự viết của Derived phải gọi bản của Base: `Derived(const Derived &d) : Base{d} {...}`, `Base::operator=(rhs);`. Nếu quên, phần Base sẽ được default-construct (constructor) hoặc giữ nguyên (operator=).
- **Redefine** hàm (cùng chữ ký, không virtual) → static binding: chọn theo kiểu tĩnh của biến/con trỏ. Gọi bản của lớp cha: `Base::f()`.
- Hàm cùng tên ở Derived **che** (hide) mọi overload cùng tên của Base → dùng `using Base::f;`.
- Đa kế thừa: `class C : public A, public B`. Diamond → `virtual` inheritance.

### Đa hình (section 16)
- **Compile-time (static) polymorphism**: overload hàm, overload toán tử, template. **Runtime (dynamic) polymorphism**: hàm virtual gọi qua con trỏ/tham chiếu tới Base.
- `virtual` ở Base → hàm tương ứng ở mọi lớp con đều virtual (có ghi lại hay không). Gọi qua object (không phải con trỏ/reference) thì luôn là static binding.
- Cơ chế: mỗi class có hàm virtual có một **vtable** (bảng con trỏ hàm); mỗi object có một **vptr** trỏ tới vtable của class thật → `sizeof` tăng thêm 8 byte.
- **Virtual destructor**: Base dùng cho đa hình phải có `virtual ~Base()`, nếu không thì `delete basePtr` là UB (thường chỉ gọi `~Base`).
- `override` (C++11): báo lỗi nếu hàm không thực sự override (sai chữ ký, thiếu const, Base không virtual). `final`: cấm override tiếp (trên hàm) hoặc cấm kế thừa (trên class).
- Gọi qua **reference** tới Base cũng đa hình như qua con trỏ.
- **Pure virtual** `virtual void draw() = 0;` → class **abstract**: không tạo object được, nhưng khai báo con trỏ/tham chiếu được. Lớp con không override hết hàm pure virtual thì vẫn là abstract.
- **Interface**: abstract class chỉ có hàm pure virtual (và virtual destructor), không có dữ liệu. Ví dụ `I_Printable` với `virtual void print(ostream&) const = 0;`, rồi `operator<<` gọi `obj.print(os)` → một `operator<<` dùng cho mọi lớp con.
- **Object slicing**: gán/truyền Derived theo giá trị vào Base → mất phần Derived, mất đa hình. Lưu đa hình trong container bằng `vector<Base*>` hoặc `vector<unique_ptr<Base>>`.
- Tham số mặc định của hàm virtual lấy theo **kiểu tĩnh** (lab `02-cpp/10`). Gọi hàm virtual trong constructor/destructor chạy bản của lớp đang dựng (lab `02-cpp/04`).

## Trắc nghiệm

### Khác nhau DUY NHẤT giữa `struct` và `class` trong C++?
- [ ] struct không có member function
- [ ] struct không kế thừa được
- [x] Quyền truy cập mặc định (member và kiểu kế thừa): struct là public, class là private
- [ ] struct cấp phát trên stack, class cấp phát trên heap
> Ngoài quyền truy cập mặc định, hai từ khoá có khả năng giống hệt nhau.

### Đoạn code có lỗi gì?
```cpp
class Player {
    std::string name;
public:
    Player(std::string n) : name{n} {}
};
int main() { Player p; }
```
- [ ] Không lỗi
- [x] Lỗi biên dịch: không có default constructor
- [ ] Lỗi linker
- [ ] name chưa khởi tạo
> Compiler chỉ tự sinh default constructor khi class không khai báo constructor nào. Sửa bằng `Player() = default;` hoặc thêm tham số mặc định.

### Đoạn code in ra gì?
```cpp
struct P {
    P() : P{0} { cout << "A"; }
    P(int) { cout << "B"; }
};
int main() { P p; }
```
- [ ] A
- [ ] AB
- [x] BA
- [ ] B
> Delegating constructor: constructor đích `P(int)` chạy xong trước, rồi mới tới thân của `P()`.

### Member nào bắt buộc phải khởi tạo bằng initializer list? (chọn tất cả đáp án đúng)
- [x] Member `const int`
- [x] Member kiểu reference `int&`
- [x] Member là object của class không có default constructor
- [ ] Member `std::string`
> Ba loại trên không thể "tạo trước rồi gán sau". `std::string` có default constructor nên gán trong thân cũng được (chỉ kém hiệu quả).

### Có bao nhiêu lần copy constructor được gọi (không tính copy elision)?
```cpp
void show(Player p);
Player a{"A"};
Player b = a;
show(b);
b = a;
```
- [ ] 1
- [x] 2
- [ ] 3
- [ ] 0
> `Player b = a` và truyền theo giá trị vào `show` gọi copy constructor. `b = a` (b đã tồn tại) gọi copy assignment.

### Vì sao copy constructor phải nhận tham số `const T&` chứ không phải `T`?
- [ ] Để nhanh hơn
- [x] Truyền theo giá trị lại cần gọi copy constructor → đệ quy vô hạn (compiler báo lỗi)
- [ ] Vì chuẩn bắt buộc mọi tham số phải là reference
- [ ] Để không sửa được object nguồn
> `const` giúp nhận được cả object const và giá trị tạm; reference mới là thứ tránh đệ quy.

### Class có member `int *data` cấp phát bằng new, dùng copy constructor mặc định. Copy object rồi cả hai bị huỷ (destructor delete data). Kết quả?
- [ ] Hoạt động bình thường
- [ ] Memory leak
- [x] Double free (undefined behavior, thường crash)
- [ ] Lỗi biên dịch
> Shallow copy làm hai object cùng giữ một địa chỉ. Cần deep copy (rule of three) hoặc dùng `unique_ptr`/`vector`.

### Move constructor thường làm gì?
- [ ] Copy sâu dữ liệu sang object mới
- [x] Lấy con trỏ tài nguyên của nguồn rồi đặt con trỏ nguồn thành nullptr
- [ ] Xoá object nguồn
- [ ] Gọi copy constructor
> Không cấp phát mới nên rẻ. Nguồn phải ở trạng thái hợp lệ để destructor của nó vẫn chạy an toàn.

### Hàm member `get_name()` cần điều kiện gì để gọi được trên `const Player p`?
- [ ] Phải là static
- [x] Phải khai báo `const`: `std::string get_name() const;`
- [ ] Phải là friend
- [ ] Phải trả về const
> Object const chỉ gọi được hàm member const, vì `this` của nó có kiểu `const Player*`.

### Điều nào ĐÚNG về static member function?
- [ ] Có con trỏ this
- [ ] Truy cập được mọi member thường
- [x] Không có this, chỉ truy cập trực tiếp static member, gọi được bằng `Class::f()` mà không cần object
- [ ] Không gọi được từ object
> Có thể gọi qua object (`p.f()`), nhưng vẫn không có this.

### Static data member `static int count;` khai báo trong class (trước C++17) cần thêm gì?
- [ ] Không cần gì
- [x] Định nghĩa đúng một lần trong một file .cpp: `int Player::count = 0;`
- [ ] Khởi tạo trong constructor
- [ ] Khai báo extern
> Không định nghĩa thì dùng tới sẽ bị lỗi linker "undefined reference". C++17 có thể viết `inline static int count = 0;` ngay trong class.

### Toán tử nào KHÔNG overload được? (chọn tất cả đáp án đúng)
- [x] `::`
- [x] `?:`
- [ ] `[]`
- [x] `.`
> Không overload được `::`, `.`, `.*`, `?:`, `sizeof`. `[]` thì overload được (và bắt buộc là member).

### Vì sao `operator<<` cho `cout << obj` phải là hàm non-member?
- [ ] Vì member function không được trả về reference
- [x] Vì vế trái là `std::ostream`, không phải class của mình
- [ ] Vì `<<` là toán tử một ngôi
- [ ] Vì friend chạy nhanh hơn
> Với member, vế trái luôn là `*this`. Nếu viết thành member thì phải gọi `obj << cout`.

### Chữ ký nào đúng cho postfix `++`?
- [ ] `T &operator++();`
- [x] `T operator++(int);`
- [ ] `T &operator++(int);`
- [ ] `T operator++(T);`
> Tham số `int` giả để phân biệt với prefix. Postfix trả về bản copy giá trị cũ, nên trả về theo giá trị.

### Copy assignment operator nên trả về gì?
- [ ] void
- [ ] T theo giá trị
- [x] `T&` (`*this`)
- [ ] `const T*`
> Trả về `*this` để nối chuỗi `a = b = c` như kiểu dựng sẵn và tránh copy thừa.

### Vì sao copy assignment cần kiểm tra `if (this == &rhs) return *this;`?
- [ ] Để nhanh hơn
- [x] Tự gán `a = a` sẽ giải phóng tài nguyên rồi copy từ chính vùng vừa giải phóng
- [ ] Chuẩn bắt buộc
- [ ] Để tránh gọi destructor
> Một cách khác là copy-and-swap idiom: an toàn khi tự gán và khi có exception.

### `Vec v = 5;` biên dịch được khi nào?
- [ ] Không bao giờ
- [x] Khi Vec có constructor nhận một int và không có `explicit`
- [ ] Khi Vec overload `operator=` nhận int
- [ ] Khi Vec có `operator int()`
> Constructor một tham số không explicit là chuyển đổi ngầm. Thêm `explicit` thì phải viết `Vec v{5};`.

### Với public inheritance, member `protected` của Base trở thành gì trong Derived?
- [ ] public
- [x] protected
- [ ] private
- [ ] Không truy cập được
> Public inheritance giữ nguyên public → public, protected → protected. Private của Base không bao giờ truy cập trực tiếp được từ Derived.

### Thứ tự gọi khi tạo `C c;` với `class C : public B`, `class B : public A`?
- [ ] C, B, A
- [x] A, B, C
- [ ] B, A, C
- [ ] Chỉ C
> Constructor đi từ lớp gốc xuống. Destructor đi ngược lại: C, B, A.

### Lớp con KHÔNG kế thừa gì từ lớp cha? (chọn tất cả đáp án đúng)
- [x] Constructor
- [x] Destructor
- [x] Copy assignment operator
- [ ] Hàm member public thông thường
> Muốn dùng lại constructor của lớp cha thì viết `using Base::Base;` (C++11).

### `Account::withdraw` KHÔNG virtual, `Savings` redefine nó. Đoạn code gọi bản nào?
```cpp
Savings s;
Account &ref = s;
ref.withdraw(100);
```
- [x] `Account::withdraw`
- [ ] `Savings::withdraw`
- [ ] Lỗi biên dịch
- [ ] Ambiguous
> Không virtual → static binding theo kiểu của ref (`Account&`). Nếu có virtual thì gọi `Savings::withdraw`.

### Trong lớp con, làm sao gọi bản `display()` của lớp cha?
- [ ] `super.display()`
- [ ] `base->display()`
- [x] `Base::display()`
- [ ] `this->Base.display()`
> C++ không có từ khoá `super`; dùng toán tử phạm vi với tên lớp cha.

### Copy constructor tự viết của Derived không gọi `Base{other}` trong initializer list. Phần Base của bản copy được tạo thế nào?
- [ ] Copy từ other
- [x] Bằng default constructor của Base
- [ ] Không được khởi tạo
- [ ] Lỗi biên dịch
> Phần Base không được copy → bug âm thầm. Phải viết `Derived(const Derived &d) : Base{d}, ...`.

### Đâu là đa hình lúc biên dịch (static polymorphism)? (chọn tất cả đáp án đúng)
- [x] Overload hàm
- [x] Overload toán tử
- [x] Template
- [ ] Gọi hàm virtual qua con trỏ Base
> Hàm virtual qua con trỏ/tham chiếu là đa hình lúc chạy (dynamic binding qua vtable).

### Từ khoá `override` giúp gì?
- [ ] Làm hàm thành virtual ở lớp cha
- [x] Compiler báo lỗi nếu hàm không thực sự override một hàm virtual của lớp cha
- [ ] Hàm chạy nhanh hơn
- [ ] Cấm lớp con override tiếp
> Bắt được lỗi sai chữ ký (thiếu const, khác kiểu tham số) mà nếu không có override thì chỉ âm thầm tạo hàm mới. Cấm override tiếp là việc của `final`.

### Class có `virtual void draw() = 0;` thì sao?
- [ ] Không có gì đặc biệt
- [x] Là abstract class: không tạo object được, nhưng khai báo được con trỏ/tham chiếu
- [ ] Không có lớp con nào được kế thừa
- [ ] Lỗi biên dịch vì hàm không có thân
> Lớp con phải override mọi hàm pure virtual mới tạo được object (concrete class).

### `final` đặt sau tên class (`class Trust final : public Savings`) nghĩa là?
- [ ] Không tạo object được
- [x] Không class nào kế thừa được từ Trust
- [ ] Mọi hàm của Trust là virtual
- [ ] Trust là abstract
> Đặt `final` sau một hàm virtual thì cấm lớp con override riêng hàm đó.

### Đoạn code in ra gì? (`speak` là virtual)
```cpp
vector<Animal> zoo;
zoo.push_back(Dog{});
zoo[0].speak();
```
- [ ] Dog::speak
- [x] Animal::speak
- [ ] Lỗi biên dịch
- [ ] Undefined behavior
> `vector<Animal>` lưu theo giá trị → Dog bị slicing thành Animal. Muốn đa hình phải dùng `vector<unique_ptr<Animal>>` hoặc `vector<Animal*>`.

### Mỗi object của class có hàm virtual tốn thêm bao nhiêu bộ nhớ (x86-64, kế thừa đơn)?
- [ ] Một con trỏ cho mỗi hàm virtual
- [x] Một con trỏ vptr (8 byte, có thể thêm padding)
- [ ] Không tốn thêm gì
- [ ] Cả một vtable
> vtable dùng chung cho cả class; mỗi object chỉ giữ một vptr trỏ tới vtable.
