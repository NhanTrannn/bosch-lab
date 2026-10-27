/*
 * Bài: Rule of five: copy/move ctor và assignment
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Với các dòng đánh số 1–7, dòng nào gọi ctor, copy ctor, copy assign, move ctor, move assign? Thứ tự các dòng dtor ở cuối?
 * Ý chính: Khởi tạo mới → constructor; gán cho object đã tồn tại → operator=. std::move chỉ ép kiểu sang rvalue. C++17 bắt buộc copy elision khi trả về prvalue (RVO).
 * Thử thêm: Bỏ noexcept ở move ctor, cho vào vector<Str> rồi push_back nhiều lần: vector dùng copy hay move khi cấp phát lại?
 */
#include <iostream>
#include <cstring>
#include <utility>
using namespace std;

class Str {
    char *data;
public:
    Str(const char *s = "") : data(new char[strlen(s) + 1])
    { strcpy(data, s); cout << "ctor(" << data << ")\n"; }
    ~Str() { cout << "dtor(" << (data ? data : "null") << ")\n"; delete[] data; }

    Str(const Str &o) : data(new char[strlen(o.data) + 1])
    { strcpy(data, o.data); cout << "copy ctor\n"; }
    Str &operator=(const Str &o) {
        cout << "copy assign\n";
        if (this != &o) {
            char *t = new char[strlen(o.data) + 1];
            strcpy(t, o.data);
            delete[] data;
            data = t;
        }
        return *this;
    }
    Str(Str &&o) noexcept : data(o.data) { o.data = nullptr; cout << "move ctor\n"; }
    Str &operator=(Str &&o) noexcept {
        cout << "move assign\n";
        if (this != &o) { delete[] data; data = o.data; o.data = nullptr; }
        return *this;
    }
};

Str make() { return Str("tmp"); }

int main()
{
    Str a("A");            // 1
    Str b = a;             // 2
    Str c;                 // 3
    c = a;                 // 4
    Str d = std::move(a);  // 5
    c = Str("X");          // 6
    Str e = make();        // 7
    cout << "--- end ---\n";
}
