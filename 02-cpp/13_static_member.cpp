/*
 * Lab: static members, static locals and the singleton
 * Topic: C++
 * Predict before running: In what order do the "create/copy/destroy" lines appear? Does make() call copy? How many times is Logger() printed? What is sizeof(Counter)?
 * Key idea: A static member is shared by all objects, is not part of sizeof, and must be defined outside the class (or inline static since C++17). A static member function has no this. A static local is initialized once, thread-safely since C++11 → the Meyers singleton. C++17 guarantees copy elision when returning a prvalue.
 * Try next: Change make() to Counter c; return c; (NRVO, usually still no copy). Compile with -fno-elide-constructors.
 */
#include <iostream>
using namespace std;

class Counter {
    static int alive;
    static int created;
    int id;

public:
    Counter() : id(++created) { ++alive; cout << "create #" << id << '\n'; }
    Counter(const Counter &o) : id(++created) { ++alive; cout << "copy #" << o.id << " -> #" << id << '\n'; }
    ~Counter() { --alive; cout << "destroy #" << id << '\n'; }
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
    cout << "n = " << Logger::get().n << ", same object? " << (&Logger::get() == &Logger::get()) << '\n';
    cout << "sizeof(Counter) = " << sizeof(Counter) << '\n';
    return 0;
}
