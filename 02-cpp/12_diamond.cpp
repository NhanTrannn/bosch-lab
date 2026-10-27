/*
 * Bài: Đa kế thừa, diamond và virtual inheritance
 * Chủ đề: C++
 * Dự đoán trước khi chạy: D d; in thứ tự constructor nào? A được tạo mấy lần? Với VD thì sao? Thứ tự E (khai báo C trước B)?
 * Ý chính: Lớp cơ sở được khởi tạo theo thứ tự KHAI BÁO trong danh sách kế thừa (không theo initializer list). Diamond thường → 2 bản A, truy cập x bị mơ hồ. Kế thừa virtual → chỉ 1 bản A, và lớp CUỐI CÙNG (most derived) gọi constructor của A.
 * Cảnh báo biên dịch cố ý: -Wreorder ở E (viết B(), C() nhưng C vẫn được khởi tạo trước vì khai báo trước).
 * Thử thêm: Bỏ comment dòng d.x → lỗi "request for member 'x' is ambiguous". Cho A constructor có tham số và xem VD phải gọi A(...) trực tiếp.
 */
#include <iostream>
using namespace std;

struct A { int x = 0; A() { cout << "A "; } };
struct B : A { B() { cout << "B "; } };
struct C : A { C() { cout << "C "; } };
struct D : B, C { D() { cout << "D\n"; } };

struct VB : virtual A { VB() { cout << "VB "; } };
struct VC : virtual A { VC() { cout << "VC "; } };
struct VD : VB, VC { VD() { cout << "VD\n"; } };

struct E : C, B { E() : B(), C() { cout << "E\n"; } };

int main()
{
    cout << "D : ";
    D d;
    d.B::x = 1;
    d.C::x = 2;
    // d.x = 3;
    cout << "d.B::x = " << d.B::x << ", d.C::x = " << d.C::x << '\n';

    cout << "VD: ";
    VD vd;
    vd.x = 7;
    cout << "vd.VB::x = " << vd.VB::x << ", vd.VC::x = " << vd.VC::x << '\n';

    cout << "E : ";
    E e;

    cout << "sizeof: A=" << sizeof(A) << " D=" << sizeof(D) << " VD=" << sizeof(VD) << '\n';
    return 0;
}
