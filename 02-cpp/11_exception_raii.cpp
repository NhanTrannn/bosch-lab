/*
 * Lab: Exceptions, stack unwinding and RAII
 * Topic: C++
 * Predict before running: In what order do the +/− lines appear? Which catch handles each exception? What does what() print when caught by value? Does ~Bad run?
 * Key idea: When an exception is thrown, local objects are destroyed in reverse order (stack unwinding) — the foundation of RAII (lock_guard, unique_ptr...). Catch exceptions by const reference to avoid slicing. If a constructor throws, the object's own destructor does NOT run, but the members already built are destroyed.
 * Intentional compiler warning: -Wcatch-value (catching std::exception by value).
 * Try next: Throw from a destructor during unwinding → std::terminate. Mark a function noexcept and throw inside it.
 */
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

struct Guard {
    string name;
    Guard(string n) : name(n) { cout << "+" << name << '\n'; }
    ~Guard() { cout << "-" << name << '\n'; }
};

struct MyError : runtime_error {
    using runtime_error::runtime_error;
};

void level2()
{
    Guard g("L2");
    throw MyError("error in level2");
    cout << "never printed\n";
}

void level1()
{
    Guard g("L1");
    try {
        level2();
    } catch (const logic_error &) {
        cout << "level1 caught it\n";
    }
    cout << "level1 finished normally\n";
}

struct Bad {
    Guard member{"member"};
    Bad() { throw 1; }
    ~Bad() { cout << "~Bad\n"; }
};

int main()
{
    try {
        Guard g("main");
        level1();
    } catch (const exception &e) {
        cout << "main caught: " << e.what() << '\n';
    }

    try {
        throw MyError("chi tiet");
    } catch (exception e) {
        cout << "caught by value: " << e.what() << '\n';
    }

    try {
        throw 42;
    } catch (double) {
        cout << "double\n";
    } catch (int x) {
        cout << "int " << x << '\n';
    } catch (...) {
        cout << "...\n";
    }

    try {
        Bad b;
    } catch (int) {
        cout << "Bad's constructor threw\n";
    }
    return 0;
}
