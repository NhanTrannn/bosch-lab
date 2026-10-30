/*
 * Lab: virtual, calls through an object/pointer/reference, and object slicing
 * Topic: C++
 * Predict before running: Does each line print "Animal", "Woof" or "name: ..."? What are sizeof(Animal) and sizeof(Dog)?
 * Key idea: Dynamic dispatch only happens when a virtual function is called through a pointer or reference. Assigning/passing by value cuts off the Derived part (slicing).
 * Try next: Remove the virtual in front of speak() in Animal and predict everything again.
 */
#include <iostream>
using namespace std;

struct Animal {
    virtual void speak() const { cout << "Animal\n"; }
    void name() const { cout << "name: Animal\n"; }
    virtual ~Animal() = default;
};

struct Dog : Animal {
    void speak() const override { cout << "Woof\n"; }
    void name() const { cout << "name: Dog\n"; }
};

void byValue(Animal a) { a.speak(); }
void byRef(const Animal &a) { a.speak(); }
void byPtr(const Animal *a) { a->speak(); }

int main()
{
    Dog d;
    Animal a = d;
    Animal &r = d;
    Animal *p = &d;

    a.speak();
    r.speak();
    p->speak();
    p->name();
    byValue(d);
    byRef(d);
    byPtr(&d);
    cout << sizeof(Animal) << ' ' << sizeof(Dog) << '\n';
}
