/*
 * Lab: Operator overloading: + - == ++ (prefix/postfix) += [] << and non-member functions
 * Topic: C++ OOP (Udemy section 14)
 * Predict before running: After c = a++, what are c and a? How many [pre++] lines does a++ print? Does 2 * a compile, and a * 2? What does (a += b) += b produce?
 * Key idea: Postfix takes a dummy int parameter and returns a copy of the OLD value (by value); prefix returns *this by reference → prefix is cheaper. Operators whose left operand is not your class (2 * v, cout << v) must be non-member functions (usually friends). Assignment/compound assignment operators return T& so they can be chained.
 * Try next: Add Vec operator*(const Vec&, int) so a * 2 works. These cannot be overloaded: . .* :: ?: sizeof.
 */
#include <iostream>
using namespace std;

class Vec {
    int x, y;

public:
    Vec(int x = 0, int y = 0) : x{x}, y{y} {}

    Vec operator+(const Vec &o) const { return {x + o.x, y + o.y}; }
    Vec operator-() const { return {-x, -y}; }
    bool operator==(const Vec &o) const { return x == o.x && y == o.y; }
    bool operator!=(const Vec &o) const { return !(*this == o); }

    Vec &operator++()
    {
        ++x; ++y;
        cout << "[pre++] ";
        return *this;
    }
    Vec operator++(int)
    {
        Vec old = *this;
        ++*this;
        cout << "[post++] ";
        return old;
    }
    Vec &operator+=(const Vec &o)
    {
        x += o.x; y += o.y;
        return *this;
    }
    int &operator[](int i) { return i == 0 ? x : y; }

    friend ostream &operator<<(ostream &os, const Vec &v) { return os << '(' << v.x << ',' << v.y << ')'; }
    friend Vec operator*(int k, const Vec &v) { return {k * v.x, k * v.y}; }
};

int main()
{
    Vec a{1, 2}, b{3, 4};
    cout << "a + b = " << a + b << ", -a = " << -a << '\n';
    cout << "a == Vec{1,2} ? " << (a == Vec{1, 2}) << ", a != b ? " << (a != b) << '\n';

    Vec c = a++;
    cout << "\nc = a++: c = " << c << ", a = " << a << '\n';
    Vec d = ++a;
    cout << "\nd = ++a: d = " << d << ", a = " << a << '\n';

    cout << "2 * a = " << 2 * a << '\n';
    // cout << a * 2;   // error: there is no operator*(Vec, int)

    a[1] = 9;
    cout << "a[1] = 9 -> a = " << a << '\n';

    Vec e{1, 1};
    (e += b) += b;
    cout << "(e += b) += b -> e = " << e << '\n';

    Vec f = 5;   // non-explicit constructor: 5 -> Vec(5, 0)
    cout << "Vec f = 5 -> " << f << ", f + 1 = " << f + 1 << '\n';
    return 0;
}
