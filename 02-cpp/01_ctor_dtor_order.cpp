/*
 * Bài: Thứ tự constructor / destructor
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Thứ tự các dòng ctor/dtor? m1 hay m2 được tạo trước? a bằng bao nhiêu?
 * Ý chính: Thứ tự khởi tạo: base → member THEO THỨ TỰ KHAI BÁO (không theo initializer list) → thân ctor. Destructor theo thứ tự ngược lại.
 * Thử thêm: Đổi khai báo thành int b, a; rồi chạy lại: a có còn là rác không?
 * Cảnh báo biên dịch cố ý: -Wreorder (initializer list khác thứ tự khai báo); bật -O2 sẽ có thêm -Wuninitialized (a dùng b chưa khởi tạo).
 */
#include <iostream>
using namespace std;

struct Member {
    int id;
    Member(int i) : id(i) { cout << "Member(" << id << ") ctor\n"; }
    ~Member() { cout << "Member(" << id << ") dtor\n"; }
};

struct Base {
    Base() { cout << "Base ctor\n"; }
    ~Base() { cout << "Base dtor\n"; }
};

struct Derived : Base {
    Member m2;
    Member m1;
    int a, b;
    Derived() : m1(1), m2(2), b(10), a(b + 1)
    {
        cout << "Derived ctor, b = " << b << ", a = " << a << '\n';
    }
    ~Derived() { cout << "Derived dtor\n"; }
};

int main()
{
    cout << "--- tao d ---\n";
    {
        Derived d;
        cout << "--- ra khoi scope ---\n";
    }
    cout << "--- het ---\n";
}
