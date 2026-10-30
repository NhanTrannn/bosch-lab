/*
 * Lab: Virtual and non-virtual destructors
 * Topic: C++
 * Predict before running: In each "---" section, which destructors are called?
 * Key idea: Deleting a Derived object through a Base pointer when ~Base() is not virtual is undefined behavior: usually only ~Base runs and Derived's resources leak.
 * Try next: Run valgrind --leak-check=full ./build/cpp/03_virtual_dtor to see the 400 leaked bytes.
 */
#include <iostream>
using namespace std;

struct BaseNV {
    ~BaseNV() { cout << "~BaseNV\n"; }
};
struct DerivedNV : BaseNV {
    int *buf = new int[100];
    ~DerivedNV() { delete[] buf; cout << "~DerivedNV\n"; }
};

struct BaseV {
    virtual ~BaseV() { cout << "~BaseV\n"; }
};
struct DerivedV : BaseV {
    int *buf = new int[100];
    ~DerivedV() override { delete[] buf; cout << "~DerivedV\n"; }
};

int main()
{
    cout << "--- delete through BaseNV* ---\n";
    BaseNV *p1 = new DerivedNV;
    delete p1;

    cout << "--- delete through BaseV* ---\n";
    BaseV *p2 = new DerivedV;
    delete p2;

    cout << "--- object on the stack ---\n";
    DerivedNV d;
}
