/*
 * Lab: Lambda capture by value and by reference
 * Topic: C++
 * Predict before running: What do byVal and byRef print after x changes to 20? What does the third inc() return, and what is the outer counter? What about all() and the last line?
 * Key idea: [x] copies the value AT THE MOMENT THE LAMBDA IS CREATED; [&x] refers to the original variable. mutable lets the lambda modify its own copy.
 * Try next: Return a [&] lambda that captures a local from a function and call it: what happens (dangling reference)?
 */
#include <iostream>
#include <functional>
#include <vector>
using namespace std;

int main()
{
    int x = 10;
    auto byVal = [x]() { return x; };
    auto byRef = [&x]() { return x; };
    x = 20;
    cout << "byVal = " << byVal() << ", byRef = " << byRef() << '\n';

    int counter = 0;
    auto inc = [counter]() mutable { return ++counter; };
    inc();
    inc();
    cout << "inc() = " << inc() << ", counter = " << counter << '\n';

    auto all = [=]() { return x + counter; };
    auto allRef = [&]() { counter += 5; };
    allRef();
    cout << "all() = " << all() << ", counter = " << counter << '\n';

    vector<function<int()>> fs;
    for (int i = 0; i < 3; i++)
        fs.push_back([i]() { return i * i; });
    for (auto &f : fs)
        cout << f() << ' ';
    cout << '\n';
}
