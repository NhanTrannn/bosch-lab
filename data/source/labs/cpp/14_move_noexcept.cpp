/*
 * Lab: std::move, const and noexcept when a vector reallocates
 * Topic: C++
 * Predict before running: When the vector grows its capacity, are the old elements copied or moved for A and for B? What does std::move(ca) call? What is left in s after it is moved from?
 * Key idea: std::move is only a cast to an rvalue reference; by itself it moves nothing. A const object cannot be moved → it falls back to copying. vector only moves elements when reallocating if the move constructor is noexcept (to keep the strong exception guarantee); otherwise it copies.
 * Try next: Add noexcept to A(A&&) and run again. Try return std::move(local); and look at the -Wpessimizing-move warning.
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct A {
    A() {}
    A(const A &) { cout << "copy "; }
    A(A &&) { cout << "move "; }
};

struct B {
    B() {}
    B(const B &) { cout << "copy "; }
    B(B &&) noexcept { cout << "move "; }
};

int main()
{
    vector<A> va;
    va.reserve(2);
    va.emplace_back();
    va.emplace_back();
    cout << "A, growing capacity: ";
    va.emplace_back();
    cout << '\n';

    vector<B> vb;
    vb.reserve(2);
    vb.emplace_back();
    vb.emplace_back();
    cout << "B, growing capacity: ";
    vb.emplace_back();
    cout << '\n';

    cout << "push_back(lvalue): ";
    B b1;
    vector<B> v2;
    v2.reserve(10);
    v2.push_back(b1);
    cout << "| push_back(std::move): ";
    v2.push_back(std::move(b1));
    cout << "| emplace_back(): ";
    v2.emplace_back();
    cout << '\n';

    const A ca;
    cout << "A x = std::move(ca): ";
    A x = std::move(ca);
    (void)x;
    cout << '\n';

    string s = "hello";
    string t = std::move(s);
    cout << "s = [" << s << "], t = [" << t << "]\n";
    return 0;
}
