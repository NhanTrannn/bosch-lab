/*
 * Lab: Calling virtual functions in constructors/destructors
 * Topic: C++
 * Predict before running: Inside the Base ctor and Base dtor, which version of hello() is called?
 * Key idea: While Base is being constructed (or destroyed) the object only "is" a Base: virtual calls do not reach Derived.
 * Try next: Make Base::hello pure virtual (= 0) and build: what do the compiler/linker report? What happens at run time?
 */
#include <iostream>
#include <string>
using namespace std;

struct Base {
    Base() { cout << "Base ctor -> "; hello(); }
    virtual ~Base() { cout << "Base dtor -> "; hello(); }
    virtual void hello() const { cout << "Base::hello\n"; }
    void call() const { hello(); }
};

struct Derived : Base {
    string msg = "Derived::hello";
    Derived() { cout << "Derived ctor -> "; hello(); }
    ~Derived() override { cout << "Derived dtor -> "; hello(); }
    void hello() const override { cout << msg << '\n'; }
};

int main()
{
    Derived d;
    cout << "call() -> ";
    d.call();
}
