/*
 * Bài: Biến, kiểu cơ bản, chia nguyên và chuyển kiểu
 * Chủ đề: C++ cơ bản (Udemy section 6, 8)
 * Dự đoán trước khi chạy: 7/2, 7%2, -7/2, -7%2? avg = (1+2+4)/3 bằng mấy? 'A'+1 in ra gì? unsigned 0 trừ 1? 10/3*3 và 10*3/3?
 * Ý chính: int / int là chia nguyên (cắt về 0), phải có ít nhất một toán hạng double mới chia thực. static_cast áp dụng TRƯỚC hay SAU phép chia cho kết quả khác nhau. char là số: 'A' + 1 lên int. {} (list init) chặn narrowing, = thì không.
 * Thử thêm: Bỏ comment dòng int bad{3.99}; để xem lỗi narrowing. Đổi avg thành (1+2+4)/3.0.
 */
#include <iostream>
using namespace std;

int main()
{
    int a = 7, b = 2;
    cout << "7/2 = " << a / b << ", 7%2 = " << a % b << ", -7/2 = " << -7 / 2 << ", -7%2 = " << -7 % 2 << '\n';
    cout << "a/2.0 = " << a / 2.0 << ", static_cast<double>(a)/b = " << static_cast<double>(a) / b
         << ", static_cast<double>(a/b) = " << static_cast<double>(a / b) << '\n';

    double avg = (1 + 2 + 4) / 3;
    cout << "avg = " << avg << '\n';
    cout << "10/3*3 = " << 10 / 3 * 3 << ", 10*3/3 = " << 10 * 3 / 3 << '\n';

    int t = 3.99;                    // = cho phép cắt (không lỗi)
    // int bad{3.99};                // {} báo lỗi narrowing
    cout << "int t = 3.99 -> " << t << ", (int)-3.99 = " << (int)-3.99 << '\n';

    char c = 'A';
    cout << "c + 1 = " << c + 1 << ", char(c + 1) = " << char(c + 1) << ", 'a' - 'A' = " << 'a' - 'A' << '\n';

    unsigned int u = 0;
    u--;
    cout << "unsigned 0 - 1 = " << u << '\n';

    bool flag = 42;
    cout << "bool flag = 42 -> " << flag << ", boolalpha: " << boolalpha << flag << noboolalpha << '\n';

    cout << "sizeof: char " << sizeof(char) << ", short " << sizeof(short) << ", int " << sizeof(int)
         << ", long " << sizeof(long) << ", long long " << sizeof(long long) << ", float " << sizeof(float)
         << ", double " << sizeof(double) << ", bool " << sizeof(bool) << '\n';

    const double PI = 3.14159;
    // PI = 3;                       // lỗi: gán cho biến const
    constexpr int SIZE = 2 * 5;     // tính lúc biên dịch
    int arr[SIZE] = {};
    cout << "PI = " << PI << ", SIZE = " << SIZE << ", sizeof(arr) = " << sizeof(arr) << '\n';
    return 0;
}
