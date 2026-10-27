/*
 * Bài: Lambda capture theo giá trị và tham chiếu
 * Chủ đề: C++
 * Dự đoán trước khi chạy: byVal, byRef in gì sau khi x đổi thành 20? inc() lần 3 trả về gì, counter ngoài bằng bao nhiêu? all() và dòng cuối?
 * Ý chính: [x] copy giá trị TẠI THỜI ĐIỂM TẠO lambda; [&x] tham chiếu tới biến gốc. mutable cho phép sửa bản copy bên trong lambda.
 * Thử thêm: Trả về một lambda [&] capture biến cục bộ từ hàm rồi gọi nó: chuyện gì xảy ra (dangling reference)?
 */
#include <iostream>
#include <functional>
#include <vector>
using namespace std;

int main()
{
    int x = 10;
    auto byVal = [x]() { return x; };
    auto byRef = [&x]() { return x; };
    x = 20;
    cout << "byVal = " << byVal() << ", byRef = " << byRef() << '\n';

    int counter = 0;
    auto inc = [counter]() mutable { return ++counter; };
    inc();
    inc();
    cout << "inc() = " << inc() << ", counter = " << counter << '\n';

    auto all = [=]() { return x + counter; };
    auto allRef = [&]() { counter += 5; };
    allRef();
    cout << "all() = " << all() << ", counter = " << counter << '\n';

    vector<function<int()>> fs;
    for (int i = 0; i < 3; i++)
        fs.push_back([i]() { return i * i; });
    for (auto &f : fs)
        cout << f() << ' ';
    cout << '\n';
}
