/*
 * Lab: Name hiding and using Base::f
 * Topic: C++
 * Predict before running: What do the calls on d and x print? If you uncomment d.f(1), does it compile?
 * Key idea: Declaring f in a derived class HIDES ALL of the base class's f overloads (even with different parameters). using Base::f; brings them back.
 * Try next: Make Base::f(int) virtual and override it in Fixed; call it through a Base* and look at the result.
 */
#include <iostream>
using namespace std;

struct Base {
    void f(int)    { cout << "Base::f(int)\n"; }
    void f(double) { cout << "Base::f(double)\n"; }
    void g(int)    { cout << "Base::g(int)\n"; }
};

struct Derived : Base {
    void f(const char *) { cout << "Derived::f(const char*)\n"; }
};

struct Fixed : Base {
    using Base::f;
    void f(const char *) { cout << "Fixed::f(const char*)\n"; }
};

int main()
{
    Derived d;
    d.f("hi");
    /* d.f(1); */
    d.Base::f(1);
    d.g(2);

    Fixed x;
    x.f("hi");
    x.f(1);
    x.f(2.5);
    x.f('A');
}
