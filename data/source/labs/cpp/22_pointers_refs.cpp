/*
 * Lab: Pointers and references: reseating, assigning through a reference, pointer arithmetic, passing pointers to functions
 * Topic: C++ basics (Udemy section 12)
 * Predict before running: a and b after r = b and *p = 99? *(q+2), q[3], *q+1, &arr[3]-q? Does reset_val make main's p a nullptr, and reset_ref? sizeof(p) and sizeof(r)?
 * Key idea: A reference must be initialized and can NOT be reseated: r = b assigns b's VALUE to a. A pointer is passed to a function by value: changing the address inside the function does not affect the caller (use int*& or int**). new goes with delete, new[] with delete[]. delete nullptr is safe.
 * Try next: Return an int& to a local variable from a function (gcc warns about dangling). Remove the delete[] and run under valgrind --leak-check=full.
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void reset_val(int *p) { p = nullptr; (void)p; }
void reset_ref(int *&p) { p = nullptr; }
void fill(int *arr, size_t n, int v) { for (size_t i = 0; i < n; ++i) arr[i] = v; }

int main()
{
    int a = 10, b = 20;
    int &r = a;
    r = b;
    cout << "after r = b: a = " << a << ", b = " << b << ", &r == &a ? " << (&r == &a) << '\n';
    r = 30;
    cout << "after r = 30: a = " << a << ", b = " << b << '\n';

    int *p = &a;
    p = &b;
    *p = 99;
    cout << "after p = &b; *p = 99: a = " << a << ", b = " << b << '\n';

    int arr[] = {10, 20, 30, 40};
    int *q = arr;
    cout << "*(q+2) = " << *(q + 2) << ", q[3] = " << q[3] << ", *q+1 = " << *q + 1 << ", &arr[3]-q = " << &arr[3] - q << '\n';
    ++q;
    cout << "after ++q: *q = " << *q << ", q[-1] = " << q[-1] << '\n';

    int *pa = &a;
    reset_val(pa); cout << "reset_val: pa is nullptr ? " << (pa == nullptr) << '\n';
    reset_ref(pa); cout << "reset_ref: pa is nullptr ? " << (pa == nullptr) << '\n';

    cout << "sizeof(p) = " << sizeof(p) << ", sizeof(r) = " << sizeof(r) << ", sizeof(arr) = " << sizeof(arr) << '\n';

    const int *pc = &a;   // cannot modify *pc, can change pc
    int *const cp = &a;   // can modify *cp, cannot change cp
    pc = &b;
    *cp = 1;
    cout << "*pc = " << *pc << ", *cp = " << *cp << '\n';

    int *one = new int{5};
    int *many = new int[3];
    fill(many, 3, 7);
    cout << "*one = " << *one << ", many = " << many[0] << many[1] << many[2] << '\n';
    delete one;
    delete[] many;
    one = nullptr;
    delete one;

    string s = "abc";
    string *ps = &s;
    vector<string> names{"Frank", "Larry"};
    vector<string> *pv = &names;
    cout << "ps->size() = " << ps->size() << ", (*pv)[1] = " << (*pv)[1] << ", pv->at(0) = " << pv->at(0) << '\n';

    int **pp = &p;
    **pp = 5;
    cout << "**pp = 5 -> b = " << b << '\n';
    return 0;
}
