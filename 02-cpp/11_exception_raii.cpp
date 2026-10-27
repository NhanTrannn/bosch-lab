/*
 * Bài: Exception, stack unwinding và RAII
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Thứ tự các dòng +/− ? catch nào bắt được? Bắt bằng giá trị in what() ra gì? ~Bad có chạy không?
 * Ý chính: Khi ném exception, các object local được huỷ theo thứ tự ngược (stack unwinding) — đó là nền tảng của RAII (lock_guard, unique_ptr...). Bắt exception bằng const reference để tránh slicing. Constructor ném lỗi → destructor của chính object KHÔNG chạy, nhưng các member đã tạo xong thì có.
 * Cảnh báo biên dịch cố ý: -Wcatch-value (bắt std::exception bằng giá trị).
 * Thử thêm: Ném exception trong destructor khi đang unwinding → std::terminate. Đánh dấu một hàm noexcept rồi ném bên trong.
 */
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

struct Guard {
    string name;
    Guard(string n) : name(n) { cout << "+" << name << '\n'; }
    ~Guard() { cout << "-" << name << '\n'; }
};

struct MyError : runtime_error {
    using runtime_error::runtime_error;
};

void level2()
{
    Guard g("L2");
    throw MyError("loi o level2");
    cout << "khong bao gio in\n";
}

void level1()
{
    Guard g("L1");
    try {
        level2();
    } catch (const logic_error &) {
        cout << "level1 bat duoc\n";
    }
    cout << "level1 ket thuc binh thuong\n";
}

struct Bad {
    Guard member{"member"};
    Bad() { throw 1; }
    ~Bad() { cout << "~Bad\n"; }
};

int main()
{
    try {
        Guard g("main");
        level1();
    } catch (const exception &e) {
        cout << "main bat: " << e.what() << '\n';
    }

    try {
        throw MyError("chi tiet");
    } catch (exception e) {
        cout << "bat bang gia tri: " << e.what() << '\n';
    }

    try {
        throw 42;
    } catch (double) {
        cout << "double\n";
    } catch (int x) {
        cout << "int " << x << '\n';
    } catch (...) {
        cout << "...\n";
    }

    try {
        Bad b;
    } catch (int) {
        cout << "constructor cua Bad nem loi\n";
    }
    return 0;
}
