/*
 * Lab: sizeof of classes: empty classes, the vptr, padding, static members
 * Topic: C++
 * Predict before running: What is sizeof of each struct on x86_64? Do two Empty objects share an address?
 * Key idea: An empty class has sizeof = 1 so every object has its own address. Virtual functions → one extra vptr per object (8 bytes on 64-bit); the vtable itself is shared by the whole class. Member functions and static members do not increase sizeof. Empty Base Optimization: an empty base class costs no bytes.
 * Try next: Add [[no_unique_address]] (C++20) to the Empty member. Add a second virtual function to WithVirtual — does sizeof change?
 */
#include <iostream>
using namespace std;

struct Empty {};
struct OneInt { int x; };
struct WithFunc { int x; void f() {} static int s; };
struct WithVirtual { virtual void f() {} };
struct IntVirtual { int x; virtual void f() {} };
struct DerivedEmpty : Empty { int x; };
struct TwoEmpty { Empty a, b; };
struct CDC { char a; double d; char c; };
struct DCC { double d; char a, c; };

int main()
{
    cout << "Empty        = " << sizeof(Empty) << '\n';
    cout << "OneInt       = " << sizeof(OneInt) << '\n';
    cout << "WithFunc     = " << sizeof(WithFunc) << '\n';
    cout << "WithVirtual  = " << sizeof(WithVirtual) << '\n';
    cout << "IntVirtual   = " << sizeof(IntVirtual) << '\n';
    cout << "DerivedEmpty = " << sizeof(DerivedEmpty) << '\n';
    cout << "TwoEmpty     = " << sizeof(TwoEmpty) << '\n';
    cout << "CDC          = " << sizeof(CDC) << '\n';
    cout << "DCC          = " << sizeof(DCC) << '\n';

    Empty e1, e2;
    cout << "&e1 != &e2 ? " << (&e1 != &e2) << '\n';
    return 0;
}
