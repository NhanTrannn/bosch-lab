/*
 * Lab: Rule of five: copy/move constructors and assignment
 * Topic: C++
 * Predict before running: For the numbered lines 1–7, which call the ctor, copy ctor, copy assignment, move ctor or move assignment? In what order are the dtor lines at the end?
 * Key idea: Creating a new object → a constructor; assigning to an existing object → operator=. std::move is only a cast to an rvalue. C++17 guarantees copy elision when returning a prvalue (RVO).
 * Try next: Remove noexcept from the move ctor, put Str objects in a vector<Str> and push_back many times: does vector copy or move when it reallocates?
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
