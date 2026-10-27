/*
 * Bài: sizeof của class: class rỗng, vptr, padding, static
 * Chủ đề: C++
 * Dự đoán trước khi chạy: sizeof của từng struct trên x86_64? Hai object Empty có cùng địa chỉ không?
 * Ý chính: Class rỗng có sizeof = 1 để mỗi object có địa chỉ riêng. Có hàm virtual → thêm một con trỏ vptr (8 byte trên 64-bit) mỗi object, vtable thì dùng chung cho cả class. Hàm thành viên và static member không làm tăng sizeof. Empty Base Optimization: lớp cơ sở rỗng không tốn byte.
 * Thử thêm: Thêm [[no_unique_address]] (C++20) cho member Empty. Thêm hàm virtual thứ hai vào WithVirtual — sizeof có đổi không?
 */
#include <iostream>
using namespace std;

struct Empty {};
struct OneInt { int x; };
struct WithFunc { int x; void f() {} static int s; };
struct WithVirtual { virtual void f() {} };
struct IntVirtual { int x; virtual void f() {} };
struct DerivedEmpty : Empty { int x; };
struct TwoEmpty { Empty a, b; };
struct CDC { char a; double d; char c; };
struct DCC { double d; char a, c; };

int main()
{
    cout << "Empty        = " << sizeof(Empty) << '\n';
    cout << "OneInt       = " << sizeof(OneInt) << '\n';
    cout << "WithFunc     = " << sizeof(WithFunc) << '\n';
    cout << "WithVirtual  = " << sizeof(WithVirtual) << '\n';
    cout << "IntVirtual   = " << sizeof(IntVirtual) << '\n';
    cout << "DerivedEmpty = " << sizeof(DerivedEmpty) << '\n';
    cout << "TwoEmpty     = " << sizeof(TwoEmpty) << '\n';
    cout << "CDC          = " << sizeof(CDC) << '\n';
    cout << "DCC          = " << sizeof(DCC) << '\n';

    Empty e1, e2;
    cout << "&e1 != &e2 ? " << (&e1 != &e2) << '\n';
    return 0;
}
