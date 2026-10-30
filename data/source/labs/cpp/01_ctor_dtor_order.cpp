/*
 * Lab: Constructor / destructor order
 * Topic: C++
 * Predict before running: In what order do the ctor/dtor lines appear? Is m1 or m2 created first? What is a?
 * Key idea: Initialization order: base → members IN DECLARATION ORDER (not initializer-list order) → constructor body. Destructors run in reverse.
 * Try next: Change the declaration to int b, a; and run again: is a still garbage?
 * Intentional compiler warning: -Wreorder (the initializer list differs from the declaration order); with -O2 you also get -Wuninitialized (a uses b before it is initialized).
 */
#include <iostream>
using namespace std;

struct Member {
    int id;
    Member(int i) : id(i) { cout << "Member(" << id << ") ctor\n"; }
    ~Member() { cout << "Member(" << id << ") dtor\n"; }
};

struct Base {
    Base() { cout << "Base ctor\n"; }
    ~Base() { cout << "Base dtor\n"; }
};

struct Derived : Base {
    Member m2;
    Member m1;
    int a, b;
    Derived() : m1(1), m2(2), b(10), a(b + 1)
    {
        cout << "Derived ctor, b = " << b << ", a = " << a << '\n';
    }
    ~Derived() { cout << "Derived dtor\n"; }
};

int main()
{
    cout << "--- create d ---\n";
    {
        Derived d;
        cout << "--- leaving scope ---\n";
    }
    cout << "--- end of main ---\n";
}
