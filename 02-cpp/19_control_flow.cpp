/*
 * Bài: Điều khiển luồng: range-for copy/ref, do-while, continue/break, dangling else, toán tử ?:
 * Chủ đề: C++ cơ bản (Udemy section 9)
 * Dự đoán trước khi chạy: Vector sau vòng for (auto x : v) và for (auto &x : v)? do-while chạy mấy lần khi điều kiện sai ngay? break trong vòng lặp lồng thoát mấy vòng? else thuộc if nào? (true ? 1 : 2.5) / 2?
 * Ý chính: Range-for với auto là bản COPY; muốn sửa phần tử phải auto&. do-while chạy ít nhất một lần, while có thể không lần nào. break/continue chỉ tác động vòng trong cùng. else luôn gắn với if gần nhất chưa có else. Hai nhánh của ?: được đưa về chung một kiểu.
 * Cảnh báo biên dịch cố ý: -Wdangling-else ở đoạn if (x) if (y) ... else (gcc gợi ý thêm ngoặc nhọn).
 * Thử thêm: Đặt dấu ; ngay sau for (...) và xem thân vòng chạy mấy lần. Thêm ngoặc { } để else thuộc if ngoài.
 */
#include <iostream>
#include <vector>
using namespace std;

void print(const vector<int> &v)
{
    for (int x : v) cout << x << ' ';
    cout << '\n';
}

int main()
{
    vector<int> v{1, 2, 3};
    for (auto x : v) x *= 10;
    cout << "sau for (auto x : v):  "; print(v);
    for (auto &x : v) x *= 10;
    cout << "sau for (auto &x : v): "; print(v);

    int i = 10;
    do { cout << "do-while chay 1 lan\n"; } while (i < 5);
    while (i < 5) cout << "while khong chay\n";

    cout << "continue/break: ";
    for (int k = 0; k < 10; ++k) {
        if (k % 2) continue;
        if (k > 6) break;
        cout << k << ' ';
    }
    cout << '\n';

    cout << "break trong vong long: ";
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) {
            if (c == 1) break;
            cout << r << c << ' ';
        }
    cout << '\n';

    int x = 1, y = 0;
    if (x)
        if (y)
            cout << "A\n";
    else
        cout << "else thuoc if (y)\n";

    int score = 75;
    cout << "score 75: " << (score >= 50 ? "pass" : "fail") << '\n';
    cout << "(true ? 1 : 2.5) / 2 = " << (true ? 1 : 2.5) / 2 << '\n';

    int day = 3;
    switch (day) {
    case 1: case 7: cout << "cuoi tuan\n"; break;
    case 2: case 3: case 4: case 5: case 6: cout << "ngay thuong\n"; break;
    default: cout << "khong hop le\n";
    }

    int sum = 0;
    for (int a = 1, b = 10; a < b; ++a, --b) sum += b - a;
    cout << "for 2 bien: sum = " << sum << '\n';
    return 0;
}
