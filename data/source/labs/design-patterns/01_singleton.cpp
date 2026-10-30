/*
 * Lab: Singleton: lazy Meyers singleton, deleted copy, thread-safe initialization
 * Topic: Design patterns — creational
 * Predict before running: Is "Config() constructed" printed before or after "main starts"? How many times is it printed when 4 threads call instance() at once? Are a and b the same object? Does "Eager() constructed" appear before main?
 * Key idea: A Singleton guarantees exactly one instance and a global access point: private constructor, deleted copy/move, and a static accessor. A function-local static (Meyers singleton) is created on first use and its initialization is thread-safe since C++11. A global/static member object is created before main (eager) and destroyed after main, in reverse order of construction.
 * Try next: Make the copy constructor public and write Config c = Config::instance(); — now there are two objects. Compare with the classic "if (!ptr) ptr = new T;" version under several threads (a data race).
 */
#include <iostream>
#include <string>
#include <thread>
#include <vector>
using namespace std;

class Config {
public:
    static Config &instance()
    {
        static Config inst;          // created on first call, thread-safe since C++11
        return inst;
    }
    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;

    string get(const string &key) const { return key == "mode" ? mode : "?"; }
    void set_mode(const string &m) { mode = m; }

private:
    Config() { cout << "Config() constructed\n"; }
    ~Config() { cout << "~Config()\n"; }
    string mode = "debug";
};

class Eager {
public:
    static Eager &instance() { return inst; }
    Eager(const Eager &) = delete;
private:
    Eager() { cout << "Eager() constructed\n"; }
    static Eager inst;
};
Eager Eager::inst;                   // created before main

int main()
{
    cout << "main starts\n";
    Eager::instance();

    vector<thread> ts;
    for (int i = 0; i < 4; ++i)
        ts.emplace_back([] { Config::instance(); });
    for (auto &t : ts) t.join();

    Config &a = Config::instance();
    Config &b = Config::instance();
    a.set_mode("release");
    cout << "same object? " << (&a == &b) << ", b.get(\"mode\") = " << b.get("mode") << '\n';
    // Config c = a;                 // error: copy constructor is deleted
    cout << "main ends\n";
    return 0;
}
