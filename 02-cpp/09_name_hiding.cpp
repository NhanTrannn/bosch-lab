/*
 * Bài: Name hiding và using Base::f
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Các lời gọi trên d và x in gì? Nếu bỏ comment d.f(1) thì có biên dịch được không?
 * Ý chính: Khai báo f trong lớp con CHE TẤT CẢ các overload f của lớp cha (kể cả khác tham số). using Base::f; đưa chúng trở lại.
 * Thử thêm: Thêm virtual cho Base::f(int) và override nó trong Fixed; gọi qua Base* xem kết quả.
 */
#include <iostream>
using namespace std;

struct Base {
    void f(int)    { cout << "Base::f(int)\n"; }
    void f(double) { cout << "Base::f(double)\n"; }
    void g(int)    { cout << "Base::g(int)\n"; }
};

struct Derived : Base {
    void f(const char *) { cout << "Derived::f(const char*)\n"; }
};

struct Fixed : Base {
    using Base::f;
    void f(const char *) { cout << "Fixed::f(const char*)\n"; }
};

int main()
{
    Derived d;
    d.f("hi");
    /* d.f(1); */
    d.Base::f(1);
    d.g(2);

    Fixed x;
    x.f("hi");
    x.f(1);
    x.f(2.5);
    x.f('A');
}
