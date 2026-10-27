/*
 * Bài: Template: suy luận kiểu, specialization, overload với hàm thường
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Mỗi lời gọi maxv chọn bản nào: hàm thường, template, hay bản specialization? Box<int>::count và Box<double>::count bằng mấy? Fact<5>::v?
 * Ý chính: Khi khớp như nhau, hàm thường được ưu tiên hơn template. Template không tự chuyển kiểu khi suy luận (maxv(1, 2.5) không suy ra được T). Mỗi kiểu T sinh ra một class riêng → static member riêng. Template có thể tính toán lúc biên dịch.
 * Thử thêm: Gọi maxv<double>(1, 2.5). Viết Fact bằng constexpr function thay vì template đệ quy.
 */
#include <cstring>
#include <iostream>
using namespace std;

template <typename T>
T maxv(T a, T b)
{
    cout << "template     ";
    return a > b ? a : b;
}

template <>
const char *maxv<const char *>(const char *a, const char *b)
{
    cout << "specialized  ";
    return strcmp(a, b) > 0 ? a : b;
}

int maxv(int a, int b)
{
    cout << "non-template ";
    return a > b ? a : b;
}

template <typename T>
struct Box {
    static int count;
    Box() { ++count; }
};
template <typename T>
int Box<T>::count = 0;

template <int N>
struct Fact { static const int v = N * Fact<N - 1>::v; };
template <>
struct Fact<0> { static const int v = 1; };

int main()
{
    int r1 = maxv(1, 2);              cout << r1 << '\n';
    double r2 = maxv(1.5, 2.5);       cout << r2 << '\n';
    int r3 = maxv<int>(3, 4);         cout << r3 << '\n';
    const char *r4 = maxv("abc", "abd"); cout << r4 << '\n';
    char r5 = maxv('a', 'b');         cout << r5 << '\n';
    int r6 = maxv(1, 2.5);            cout << r6 << '\n';

    Box<int> a, b;
    Box<double> c;
    cout << "Box<int>::count = " << Box<int>::count << ", Box<double>::count = " << Box<double>::count << '\n';
    cout << "Fact<5>::v = " << Fact<5>::v << '\n';
    return 0;
}
