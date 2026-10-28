/*
 * Bài: Toán tử: ++/--, gán kép, so sánh, logic và short-circuit
 * Chủ đề: C++ cơ bản (Udemy section 8)
 * Dự đoán trước khi chạy: i, j, k sau i++ và ++i? a sau chuỗi gán kép? cnt sau true || inc()? 0.1 + 0.2 == 0.3? 1 < 2 < 3 và 3 > 2 > 1? 5 / 2 * 2.0?
 * Ý chính: i++ trả về giá trị CŨ, ++i trả về giá trị MỚI. && và || dừng sớm (vế phải có thể không chạy). && ưu tiên hơn ||. Không so sánh double bằng ==. a < b < c là (a < b) < c.
 * Cảnh báo biên dịch cố ý: -Wparentheses ở true || false && false (gcc gợi ý thêm ngoặc dù && vốn ưu tiên hơn) và ở 1 < 2 < 3, 3 > 2 > 1 (gcc nhận ra cái bẫy).
 * Thử thêm: Viết if (x = 0) thay cho if (x == 0) và xem cảnh báo. Đổi thứ tự hai vế của && ở dòng chia cho n.
 */
#include <cmath>
#include <iostream>
using namespace std;

int cnt = 0;
bool inc() { ++cnt; return true; }

int main()
{
    int i = 5;
    int j = i++;
    cout << "sau j = i++: i = " << i << ", j = " << j << '\n';
    int k = ++i;
    cout << "sau k = ++i: i = " << i << ", k = " << k << '\n';

    int a = 10;
    a += 5 * 2;      // 20
    a -= 3;          // 17
    a *= 2;          // 34
    a /= 3;          // 11
    a %= 7;          // 4
    cout << "a = " << a << '\n';
    int m = 3;
    m *= 2 + 1;      // m = m * (2 + 1)
    cout << "m *= 2 + 1 -> " << m << '\n';

    int n = 0;
    bool safe = (n != 0) && (10 / n > 1);
    cout << "(n != 0) && (10 / n > 1) = " << safe << " (không chia cho 0)\n";

    bool t1 = true || inc();
    bool t2 = false && inc();
    bool t3 = false || inc();
    cout << "t1 t2 t3 = " << t1 << t2 << t3 << ", cnt = " << cnt << '\n';

    cout << "true || false && false = " << (true || false && false) << '\n';
    cout << "!0 + 1 = " << !0 + 1 << ", !(0 + 1) = " << !(0 + 1) << '\n';

    cout << "0.1 + 0.2 == 0.3 ? " << (0.1 + 0.2 == 0.3) << ", |diff| < 1e-9 ? " << (fabs(0.1 + 0.2 - 0.3) < 1e-9) << '\n';
    cout << "1 < 2 < 3 = " << (1 < 2 < 3) << ", 3 > 2 > 1 = " << (3 > 2 > 1) << '\n';
    cout << "5 / 2 * 2.0 = " << 5 / 2 * 2.0 << ", 5 / 2.0 * 2 = " << 5 / 2.0 * 2 << '\n';
    cout << "2 + 3 * 4 = " << 2 + 3 * 4 << ", (2 + 3) * 4 = " << (2 + 3) * 4 << ", 10 - 4 - 3 = " << 10 - 4 - 3 << '\n';
    return 0;
}
