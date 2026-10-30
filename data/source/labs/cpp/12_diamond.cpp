/*
 * Lab: Multiple inheritance, the diamond and virtual inheritance
 * Topic: C++
 * Predict before running: In what order does D d; print its constructors? How many times is A created? What about VD? And the order for E (which lists C before B)?
 * Key idea: Base classes are initialized in the order they are DECLARED in the base list (not initializer-list order). A plain diamond → 2 copies of A and accessing x is ambiguous. Virtual inheritance → a single A, and the MOST DERIVED class calls A's constructor.
 * Intentional compiler warning: -Wreorder in E (it writes B(), C() but C is still initialized first because it is declared first).
 * Try next: Uncomment the d.x line → "request for member 'x' is ambiguous". Give A a constructor with a parameter and see that VD must call A(...) directly.
 */
#include <iostream>
using namespace std;

struct A { int x = 0; A() { cout << "A "; } };
struct B : A { B() { cout << "B "; } };
struct C : A { C() { cout << "C "; } };
struct D : B, C { D() { cout << "D\n"; } };

struct VB : virtual A { VB() { cout << "VB "; } };
struct VC : virtual A { VC() { cout << "VC "; } };
struct VD : VB, VC { VD() { cout << "VD\n"; } };

struct E : C, B { E() : B(), C() { cout << "E\n"; } };

int main()
{
    cout << "D : ";
    D d;
    d.B::x = 1;
    d.C::x = 2;
    // d.x = 3;
    cout << "d.B::x = " << d.B::x << ", d.C::x = " << d.C::x << '\n';

    cout << "VD: ";
    VD vd;
    vd.x = 7;
    cout << "vd.VB::x = " << vd.VB::x << ", vd.VC::x = " << vd.VC::x << '\n';

    cout << "E : ";
    E e;

    cout << "sizeof: A=" << sizeof(A) << " D=" << sizeof(D) << " VD=" << sizeof(VD) << '\n';
    return 0;
}
