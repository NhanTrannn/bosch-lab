/*
 * Bài: Tham số mặc định của hàm virtual và gọi virtual qua hàm thường
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Mỗi dòng in "Base::f" hay "Derived::f", và x bằng 1 hay 2?
 * Ý chính: Hàm nào được gọi quyết định lúc CHẠY (dynamic binding qua vtable), nhưng giá trị tham số mặc định lấy theo kiểu TĨNH lúc biên dịch. Vì vậy đừng định nghĩa lại tham số mặc định ở lớp con.
 * Thử thêm: Thêm final vào Derived::f rồi viết lớp kế thừa Derived và override f → lỗi biên dịch. Bỏ virtual ở Base::f rồi chạy lại.
 */
#include <iostream>
using namespace std;

struct Base {
    virtual void f(int x = 1) { cout << "Base::f x=" << x << '\n'; }
    void run() { f(); } // hàm thường gọi hàm virtual (template method)
    virtual ~Base() = default;
};

struct Derived : Base {
    void f(int x = 2) override { cout << "Derived::f x=" << x << '\n'; }
};

int main()
{
    Derived d;
    Base *pb = &d;
    Base &rb = d;
    Base b = d; // slicing: b là Base thật

    cout << "1: "; pb->f();
    cout << "2: "; rb.f();
    cout << "3: "; d.f();
    cout << "4: "; pb->Base::f();
    cout << "5: "; d.run();
    cout << "6: "; b.f();
    cout << "7: "; static_cast<Base &>(d).f(5);
    return 0;
}
