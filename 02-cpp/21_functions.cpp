/*
 * Bài: Hàm: truyền tham trị/tham chiếu/con trỏ, tham số mặc định, overload, static local, đệ quy
 * Chủ đề: C++ cơ bản (Udemy section 11)
 * Dự đoán trước khi chạy: x sau by_val/by_ref/by_ptr? print(5.0f), print('a'), print("hi") gọi bản nào? counter() ba lần? fib(5) gọi bao nhiêu lần?
 * Ý chính: Tham trị nhận bản copy, tham chiếu/con trỏ sửa được biến gốc. Overload chọn bản khớp tốt nhất: khớp chính xác > promotion (float→double, char→int) > chuyển đổi chuẩn (const char*→bool) > chuyển đổi do người dùng định nghĩa (const char*→string). Tham số mặc định chỉ ghi ở khai báo và phải nằm cuối danh sách.
 * Thử thêm: Xoá print(bool) rồi chạy lại print("hi"). Gọi print(5L) để thấy lỗi ambiguous.
 */
#include <iostream>
#include <string>
using namespace std;

void by_val(int x) { x = 100; (void)x; }
void by_ref(int &x) { x = 200; }
void by_ptr(int *p) { *p = 300; }
void swap_val(int a, int b) { int t = a; a = b; b = t; }
void swap_ref(int &a, int &b) { int t = a; a = b; b = t; }

void print(int) { cout << "int "; }
void print(double) { cout << "double "; }
void print(char) { cout << "char "; }
void print(bool) { cout << "bool "; }
void print(const string &) { cout << "string "; }

void greet(const string &name = "Guest", int times = 1)
{
    for (int i = 0; i < times; ++i) cout << "Hi " << name << ' ';
    cout << '\n';
}

int counter()
{
    static int c = 0;   // khởi tạo một lần duy nhất
    return ++c;
}

int calls = 0;
int fib(int n)
{
    ++calls;
    return n <= 1 ? n : fib(n - 1) + fib(n - 2);
}

unsigned long long factorial(unsigned n) { return n == 0 ? 1 : n * factorial(n - 1); }

int main()
{
    int x = 1;
    by_val(x); cout << "by_val: x = " << x << '\n';
    by_ref(x); cout << "by_ref: x = " << x << '\n';
    by_ptr(&x); cout << "by_ptr: x = " << x << '\n';

    int a = 1, b = 2;
    swap_val(a, b); cout << "swap_val: " << a << ' ' << b << '\n';
    swap_ref(a, b); cout << "swap_ref: " << a << ' ' << b << '\n';

    cout << "overload: ";
    print(5); print(5.0); print(5.0f); print('a'); print(true); print("hi"); print(string("hi"));
    cout << '\n';

    greet();
    greet("Frank");
    greet("Frank", 2);

    int r1 = counter();
    int r2 = counter();
    int r3 = counter();
    cout << "counter: " << r1 << ' ' << r2 << ' ' << r3 << '\n';

    cout << "fib(5) = " << fib(5) << ", so lan goi = " << calls << '\n';
    cout << "factorial(10) = " << factorial(10) << '\n';
    return 0;
}
