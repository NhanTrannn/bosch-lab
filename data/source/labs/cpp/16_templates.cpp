/*
 * Lab: Templates: type deduction, specialization, overloading with a normal function
 * Topic: C++
 * Predict before running: Which version does each maxv call pick: the normal function, the template, or the specialization? What are Box<int>::count and Box<double>::count? Fact<5>::v?
 * Key idea: When both match equally well, a normal function beats a template. Template deduction does not convert types (maxv(1, 2.5) cannot deduce T). Each type T generates a separate class → separate static members. Templates can compute at compile time.
 * Try next: Call maxv<double>(1, 2.5). Write Fact as a constexpr function instead of a recursive template.
 */
#include <cstring>
#include <iostream>
using namespace std;

template <typename T>
T maxv(T a, T b)
{
    cout << "template     ";
    return a > b ? a : b;
}

template <>
const char *maxv<const char *>(const char *a, const char *b)
{
    cout << "specialized  ";
    return strcmp(a, b) > 0 ? a : b;
}

int maxv(int a, int b)
{
    cout << "non-template ";
    return a > b ? a : b;
}

template <typename T>
struct Box {
    static int count;
    Box() { ++count; }
};
template <typename T>
int Box<T>::count = 0;

template <int N>
struct Fact { static const int v = N * Fact<N - 1>::v; };
template <>
struct Fact<0> { static const int v = 1; };

int main()
{
    int r1 = maxv(1, 2);              cout << r1 << '\n';
    double r2 = maxv(1.5, 2.5);       cout << r2 << '\n';
    int r3 = maxv<int>(3, 4);         cout << r3 << '\n';
    const char *r4 = maxv("abc", "abd"); cout << r4 << '\n';
    char r5 = maxv('a', 'b');         cout << r5 << '\n';
    int r6 = maxv(1, 2.5);            cout << r6 << '\n';

    Box<int> a, b;
    Box<double> c;
    cout << "Box<int>::count = " << Box<int>::count << ", Box<double>::count = " << Box<double>::count << '\n';
    cout << "Fact<5>::v = " << Fact<5>::v << '\n';
    return 0;
}
