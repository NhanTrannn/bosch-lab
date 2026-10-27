/*
 * Bài: std::move, const và noexcept khi vector cấp phát lại
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Khi vector tăng capacity, phần tử cũ được copy hay move với A và B? std::move(ca) gọi gì? s sau khi bị move còn gì?
 * Ý chính: std::move chỉ là ép kiểu sang rvalue reference, bản thân nó không di chuyển gì. Object const không move được → rơi về copy. vector chỉ dùng move khi cấp phát lại nếu move constructor là noexcept (để giữ strong exception guarantee), nếu không sẽ copy.
 * Thử thêm: Thêm noexcept cho A(A&&) rồi chạy lại. Thử return std::move(local); và xem cảnh báo -Wpessimizing-move.
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct A {
    A() {}
    A(const A &) { cout << "copy "; }
    A(A &&) { cout << "move "; }
};

struct B {
    B() {}
    B(const B &) { cout << "copy "; }
    B(B &&) noexcept { cout << "move "; }
};

int main()
{
    vector<A> va;
    va.reserve(2);
    va.emplace_back();
    va.emplace_back();
    cout << "A, tang capacity: ";
    va.emplace_back();
    cout << '\n';

    vector<B> vb;
    vb.reserve(2);
    vb.emplace_back();
    vb.emplace_back();
    cout << "B, tang capacity: ";
    vb.emplace_back();
    cout << '\n';

    cout << "push_back(lvalue): ";
    B b1;
    vector<B> v2;
    v2.reserve(10);
    v2.push_back(b1);
    cout << "| push_back(std::move): ";
    v2.push_back(std::move(b1));
    cout << "| emplace_back(): ";
    v2.emplace_back();
    cout << '\n';

    const A ca;
    cout << "A x = std::move(ca): ";
    A x = std::move(ca);
    (void)x;
    cout << '\n';

    string s = "hello";
    string t = std::move(s);
    cout << "s = [" << s << "], t = [" << t << "]\n";
    return 0;
}
