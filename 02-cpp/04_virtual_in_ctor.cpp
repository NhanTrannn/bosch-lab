/*
 * Bài: Gọi hàm virtual trong constructor/destructor
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Trong Base ctor và Base dtor, hello() gọi phiên bản nào?
 * Ý chính: Trong lúc Base đang được tạo (hoặc huỷ), object chỉ "là" Base: lời gọi virtual không đi xuống Derived.
 * Thử thêm: Đổi Base::hello thành pure virtual (= 0) rồi build: compiler/linker báo gì? Chạy thì sao?
 */
#include <iostream>
#include <string>
using namespace std;

struct Base {
    Base() { cout << "Base ctor -> "; hello(); }
    virtual ~Base() { cout << "Base dtor -> "; hello(); }
    virtual void hello() const { cout << "Base::hello\n"; }
    void call() const { hello(); }
};

struct Derived : Base {
    string msg = "Derived::hello";
    Derived() { cout << "Derived ctor -> "; hello(); }
    ~Derived() override { cout << "Derived dtor -> "; hello(); }
    void hello() const override { cout << msg << '\n'; }
};

int main()
{
    Derived d;
    cout << "call() -> ";
    d.call();
}
