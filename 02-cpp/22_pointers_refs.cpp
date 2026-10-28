/*
 * Bài: Con trỏ và tham chiếu: trỏ lại, gán qua reference, pointer arithmetic, truyền con trỏ vào hàm
 * Chủ đề: C++ cơ bản (Udemy section 12)
 * Dự đoán trước khi chạy: a, b sau r = b và *p = 99? *(q+2), q[3], *q+1, &arr[3]-q? reset_val có làm p trong main thành nullptr không, còn reset_ref? sizeof(p) và sizeof(r)?
 * Ý chính: Reference phải khởi tạo và KHÔNG trỏ lại được: r = b là gán GIÁ TRỊ của b vào a. Con trỏ truyền vào hàm theo giá trị: đổi địa chỉ bên trong hàm không ảnh hưởng bên ngoài (cần int*& hoặc int**). new đi với delete, new[] đi với delete[]. delete nullptr là an toàn.
 * Thử thêm: Trả về int& tới biến local trong một hàm (gcc cảnh báo dangling). Bỏ delete[] rồi chạy với valgrind --leak-check=full.
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void reset_val(int *p) { p = nullptr; (void)p; }
void reset_ref(int *&p) { p = nullptr; }
void fill(int *arr, size_t n, int v) { for (size_t i = 0; i < n; ++i) arr[i] = v; }

int main()
{
    int a = 10, b = 20;
    int &r = a;
    r = b;
    cout << "sau r = b: a = " << a << ", b = " << b << ", &r == &a ? " << (&r == &a) << '\n';
    r = 30;
    cout << "sau r = 30: a = " << a << ", b = " << b << '\n';

    int *p = &a;
    p = &b;
    *p = 99;
    cout << "sau p = &b; *p = 99: a = " << a << ", b = " << b << '\n';

    int arr[] = {10, 20, 30, 40};
    int *q = arr;
    cout << "*(q+2) = " << *(q + 2) << ", q[3] = " << q[3] << ", *q+1 = " << *q + 1 << ", &arr[3]-q = " << &arr[3] - q << '\n';
    ++q;
    cout << "sau ++q: *q = " << *q << ", q[-1] = " << q[-1] << '\n';

    int *pa = &a;
    reset_val(pa); cout << "reset_val: pa la nullptr ? " << (pa == nullptr) << '\n';
    reset_ref(pa); cout << "reset_ref: pa la nullptr ? " << (pa == nullptr) << '\n';

    cout << "sizeof(p) = " << sizeof(p) << ", sizeof(r) = " << sizeof(r) << ", sizeof(arr) = " << sizeof(arr) << '\n';

    const int *pc = &a;   // không sửa được *pc, đổi được pc
    int *const cp = &a;   // sửa được *cp, không đổi được cp
    pc = &b;
    *cp = 1;
    cout << "*pc = " << *pc << ", *cp = " << *cp << '\n';

    int *one = new int{5};
    int *many = new int[3];
    fill(many, 3, 7);
    cout << "*one = " << *one << ", many = " << many[0] << many[1] << many[2] << '\n';
    delete one;
    delete[] many;
    one = nullptr;
    delete one;

    string s = "abc";
    string *ps = &s;
    vector<string> names{"Frank", "Larry"};
    vector<string> *pv = &names;
    cout << "ps->size() = " << ps->size() << ", (*pv)[1] = " << (*pv)[1] << ", pv->at(0) = " << pv->at(0) << '\n';

    int **pp = &p;
    **pp = 5;
    cout << "**pp = 5 -> b = " << b << '\n';
    return 0;
}
