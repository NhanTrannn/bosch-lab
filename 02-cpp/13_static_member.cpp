/*
 * Bài: static member, static local và singleton
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Các dòng "tao/copy/huy" theo thứ tự nào? make() có gọi copy không? Logger() in mấy lần? sizeof(Counter)?
 * Ý chính: static member dùng chung cho mọi object, không nằm trong sizeof object, phải định nghĩa ở ngoài class (hoặc inline static từ C++17). static member function không có this. static local khởi tạo một lần, thread-safe từ C++11 → Meyers singleton. C++17 bảo đảm copy elision khi return prvalue.
 * Thử thêm: Đổi make() thành Counter c; return c; (NRVO, thường vẫn không copy). Biên dịch với -fno-elide-constructors.
 */
#include <iostream>
using namespace std;

class Counter {
    static int alive;
    static int created;
    int id;

public:
    Counter() : id(++created) { ++alive; cout << "tao #" << id << '\n'; }
    Counter(const Counter &o) : id(++created) { ++alive; cout << "copy #" << o.id << " -> #" << id << '\n'; }
    ~Counter() { --alive; cout << "huy #" << id << '\n'; }
    static void report() { cout << "alive=" << alive << " created=" << created << '\n'; }
};

int Counter::alive = 0;
int Counter::created = 0;

Counter make() { return Counter(); }
void by_value(Counter) { Counter::report(); }

class Logger {
    Logger() { cout << "Logger()\n"; }

public:
    int n = 0;
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    static Logger &get()
    {
        static Logger inst;
        return inst;
    }
};

int main()
{
    Counter::report();
    {
        Counter a;
        Counter b = a;
        by_value(a);
        Counter c = make();
        Counter::report();
    }
    Counter::report();

    Logger::get().n++;
    Logger::get().n++;
    cout << "n = " << Logger::get().n << ", cung 1 object? " << (&Logger::get() == &Logger::get()) << '\n';
    cout << "sizeof(Counter) = " << sizeof(Counter) << '\n';
    return 0;
}
