/*
 * Bài: Lambda (capture, mutable, dùng với thuật toán) và enum / enum class
 * Chủ đề: C++ hiện đại (Udemy section 21, 23)
 * Dự đoán trước khi chạy: Gọi by_val() sau khi x = 100 in ra gì, by_ref() thì sao? counter() mutable gọi 3 lần, biến n bên ngoài đổi không? Giá trị của Color::Blue, Status::Error? Red == 0 với enum thường và enum class?
 * Ý chính: [=] chụp giá trị LÚC TẠO lambda, [&] đọc giá trị lúc GỌI. Lambda capture theo giá trị là const, cần mutable để sửa bản copy riêng (biến ngoài không đổi). Mỗi lambda có kiểu riêng; lưu bằng auto hoặc std::function. enum thường: tên tràn ra phạm vi ngoài và tự chuyển sang int; enum class: phải ghi Color::Red, không tự chuyển sang int, cần static_cast.
 * Thử thêm: Viết if (Color2::Red == 0) để thấy lỗi biên dịch. Trả về lambda [&] chụp biến local rồi gọi sau khi hàm kết thúc (dangling).
 */
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;

enum Color { Red, Green = 5, Blue };
enum class Status : char { Ok = 'O', Error = 'E' };
enum class Color2 { Red, Green, Blue };

function<int(int)> make_adder(int n) { return [n](int x) { return x + n; }; }

int main()
{
    int x = 1;
    auto by_val = [x]() { return x; };
    auto by_ref = [&x]() { return x; };
    x = 100;
    cout << "by_val() = " << by_val() << ", by_ref() = " << by_ref() << '\n';

    int n = 0;
    auto counter = [n]() mutable { return ++n; };
    int c1 = counter();
    int c2 = counter();
    int c3 = counter();
    cout << "counter mutable: " << c1 << ' ' << c2 << ' ' << c3 << ", n ngoai = " << n << '\n';

    auto copy = counter;
    cout << "copy() = " << copy() << ", counter() = " << counter() << '\n';

    vector<int> v{1, 2, 3, 4, 5, 6};
    int threshold = 3, total = 0;
    for_each(v.begin(), v.end(), [&total](int e) { total += e; });
    auto big = count_if(v.begin(), v.end(), [threshold](int e) { return e > threshold; });
    cout << "total = " << total << ", so phan tu > " << threshold << " = " << big << '\n';

    auto add5 = make_adder(5);
    auto square = [](auto a) { return a * a; };
    cout << "add5(10) = " << add5(10) << ", square(3) = " << square(3) << ", square(1.5) = " << square(1.5)
         << ", IIFE = " << [](int a, int b) { return a * b; }(6, 7) << '\n';

    cout << "Red = " << Red << ", Green = " << Green << ", Blue = " << Blue << ", Red == 0 ? " << (Red == 0) << '\n';
    Color c = Blue;
    int as_int = c;   // enum thường tự chuyển sang int
    cout << "int as_int = Blue -> " << as_int << '\n';

    Status st = Status::Error;
    cout << "Status::Error = " << static_cast<char>(st) << ", sizeof(Status) = " << sizeof(Status)
         << ", Color2::Blue = " << static_cast<int>(Color2::Blue) << '\n';
    switch (st) {
    case Status::Ok: cout << "Ok\n"; break;
    case Status::Error: cout << "Error\n"; break;
    }
    return 0;
}
