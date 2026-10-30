/*
 * Lab: Lambdas (captures, mutable, use with algorithms) and enum / enum class
 * Topic: Modern C++ (Udemy sections 21, 23)
 * Predict before running: After x = 100, what do by_val() and by_ref() print? A mutable counter() called 3 times: does the outer n change? What are Color::Blue and Status::Error? Red == 0 with a plain enum and with enum class?
 * Key idea: [=] captures the value WHEN THE LAMBDA IS CREATED, [&] reads the value WHEN IT IS CALLED. By-value captures are const; mutable lets the lambda modify its own copy (the outer variable does not change). Every lambda has its own type; store it with auto or std::function. Plain enum: names leak into the enclosing scope and convert to int implicitly; enum class: you must write Color::Red, there is no implicit conversion to int, use static_cast.
 * Try next: Write if (Color2::Red == 0) to see the compile error. Return a [&] lambda capturing a local and call it after the function has returned (dangling).
 */
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;

enum Color { Red, Green = 5, Blue };
enum class Status : char { Ok = 'O', Error = 'E' };
enum class Color2 { Red, Green, Blue };

function<int(int)> make_adder(int n) { return [n](int x) { return x + n; }; }

int main()
{
    int x = 1;
    auto by_val = [x]() { return x; };
    auto by_ref = [&x]() { return x; };
    x = 100;
    cout << "by_val() = " << by_val() << ", by_ref() = " << by_ref() << '\n';

    int n = 0;
    auto counter = [n]() mutable { return ++n; };
    int c1 = counter();
    int c2 = counter();
    int c3 = counter();
    cout << "counter mutable: " << c1 << ' ' << c2 << ' ' << c3 << ", outer n = " << n << '\n';

    auto copy = counter;
    cout << "copy() = " << copy() << ", counter() = " << counter() << '\n';

    vector<int> v{1, 2, 3, 4, 5, 6};
    int threshold = 3, total = 0;
    for_each(v.begin(), v.end(), [&total](int e) { total += e; });
    auto big = count_if(v.begin(), v.end(), [threshold](int e) { return e > threshold; });
    cout << "total = " << total << ", elements > " << threshold << " = " << big << '\n';

    auto add5 = make_adder(5);
    auto square = [](auto a) { return a * a; };
    cout << "add5(10) = " << add5(10) << ", square(3) = " << square(3) << ", square(1.5) = " << square(1.5)
         << ", IIFE = " << [](int a, int b) { return a * b; }(6, 7) << '\n';

    cout << "Red = " << Red << ", Green = " << Green << ", Blue = " << Blue << ", Red == 0 ? " << (Red == 0) << '\n';
    Color c = Blue;
    int as_int = c;   // a plain enum converts to int implicitly
    cout << "int as_int = Blue -> " << as_int << '\n';

    Status st = Status::Error;
    cout << "Status::Error = " << static_cast<char>(st) << ", sizeof(Status) = " << sizeof(Status)
         << ", Color2::Blue = " << static_cast<int>(Color2::Blue) << '\n';
    switch (st) {
    case Status::Ok: cout << "Ok\n"; break;
    case Status::Error: cout << "Error\n"; break;
    }
    return 0;
}
