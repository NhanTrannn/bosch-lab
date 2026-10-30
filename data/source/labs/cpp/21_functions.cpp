/*
 * Lab: Functions: pass by value/reference/pointer, default arguments, overloading, static locals, recursion
 * Topic: C++ basics (Udemy section 11)
 * Predict before running: x after by_val/by_ref/by_ptr? Which overload do print(5.0f), print('a') and print("hi") call? counter() three times? How many calls does fib(5) make?
 * Key idea: Pass by value gets a copy; references/pointers can modify the original. Overload resolution picks the best match: exact match > promotion (float→double, char→int) > standard conversion (const char*→bool) > user-defined conversion (const char*→string). Default arguments go only in the declaration and must be last.
 * Try next: Delete print(bool) and run print("hi") again. Call print(5L) to see the ambiguity error.
 */
#include <iostream>
#include <string>
using namespace std;

void by_val(int x) { x = 100; (void)x; }
void by_ref(int &x) { x = 200; }
void by_ptr(int *p) { *p = 300; }
void swap_val(int a, int b) { int t = a; a = b; b = t; }
void swap_ref(int &a, int &b) { int t = a; a = b; b = t; }

void print(int) { cout << "int "; }
void print(double) { cout << "double "; }
void print(char) { cout << "char "; }
void print(bool) { cout << "bool "; }
void print(const string &) { cout << "string "; }

void greet(const string &name = "Guest", int times = 1)
{
    for (int i = 0; i < times; ++i) cout << "Hi " << name << ' ';
    cout << '\n';
}

int counter()
{
    static int c = 0;   // initialized exactly once
    return ++c;
}

int calls = 0;
int fib(int n)
{
    ++calls;
    return n <= 1 ? n : fib(n - 1) + fib(n - 2);
}

unsigned long long factorial(unsigned n) { return n == 0 ? 1 : n * factorial(n - 1); }

int main()
{
    int x = 1;
    by_val(x); cout << "by_val: x = " << x << '\n';
    by_ref(x); cout << "by_ref: x = " << x << '\n';
    by_ptr(&x); cout << "by_ptr: x = " << x << '\n';

    int a = 1, b = 2;
    swap_val(a, b); cout << "swap_val: " << a << ' ' << b << '\n';
    swap_ref(a, b); cout << "swap_ref: " << a << ' ' << b << '\n';

    cout << "overload: ";
    print(5); print(5.0); print(5.0f); print('a'); print(true); print("hi"); print(string("hi"));
    cout << '\n';

    greet();
    greet("Frank");
    greet("Frank", 2);

    int r1 = counter();
    int r2 = counter();
    int r3 = counter();
    cout << "counter: " << r1 << ' ' << r2 << ' ' << r3 << '\n';

    cout << "fib(5) = " << fib(5) << ", number of calls = " << calls << '\n';
    cout << "factorial(10) = " << factorial(10) << '\n';
    return 0;
}
