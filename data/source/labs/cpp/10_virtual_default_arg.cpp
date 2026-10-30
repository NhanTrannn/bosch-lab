/*
 * Lab: Default arguments of virtual functions, and virtual calls through a normal function
 * Topic: C++
 * Predict before running: Does each line print "Base::f" or "Derived::f", and is x 1 or 2?
 * Key idea: Which function runs is decided at RUN TIME (dynamic binding through the vtable), but the default argument comes from the STATIC type at compile time. So never redefine default arguments in a derived class.
 * Try next: Add final to Derived::f, then write a class deriving from Derived that overrides f → compile error. Remove virtual from Base::f and run again.
 */
#include <iostream>
using namespace std;

struct Base {
    virtual void f(int x = 1) { cout << "Base::f x=" << x << '\n'; }
    void run() { f(); } // a normal function calling a virtual one (template method)
    virtual ~Base() = default;
};

struct Derived : Base {
    void f(int x = 2) override { cout << "Derived::f x=" << x << '\n'; }
};

int main()
{
    Derived d;
    Base *pb = &d;
    Base &rb = d;
    Base b = d; // slicing: b is a real Base

    cout << "1: "; pb->f();
    cout << "2: "; rb.f();
    cout << "3: "; d.f();
    cout << "4: "; pb->Base::f();
    cout << "5: "; d.run();
    cout << "6: "; b.f();
    cout << "7: "; static_cast<Base &>(d).f(5);
    return 0;
}
