/*
 * Bài: Destructor virtual và không virtual
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Ở mỗi đoạn "---", destructor nào được gọi?
 * Ý chính: delete một object Derived qua con trỏ Base mà ~Base() không virtual là undefined behavior: thường chỉ chạy ~Base, rò rỉ tài nguyên của Derived.
 * Thử thêm: Chạy với valgrind --leak-check=full ./build/02-cpp/03_virtual_dtor để thấy 400 byte bị leak.
 */
#include <iostream>
using namespace std;

struct BaseNV {
    ~BaseNV() { cout << "~BaseNV\n"; }
};
struct DerivedNV : BaseNV {
    int *buf = new int[100];
    ~DerivedNV() { delete[] buf; cout << "~DerivedNV\n"; }
};

struct BaseV {
    virtual ~BaseV() { cout << "~BaseV\n"; }
};
struct DerivedV : BaseV {
    int *buf = new int[100];
    ~DerivedV() override { delete[] buf; cout << "~DerivedV\n"; }
};

int main()
{
    cout << "--- delete qua BaseNV* ---\n";
    BaseNV *p1 = new DerivedNV;
    delete p1;

    cout << "--- delete qua BaseV* ---\n";
    BaseV *p2 = new DerivedV;
    delete p2;

    cout << "--- object tren stack ---\n";
    DerivedNV d;
}
