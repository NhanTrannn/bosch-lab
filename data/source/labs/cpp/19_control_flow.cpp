/*
 * Lab: Control flow: range-for copy/reference, do-while, continue/break, dangling else, the ?: operator
 * Topic: C++ basics (Udemy section 9)
 * Predict before running: What is the vector after for (auto x : v) and for (auto &x : v)? How many times does a do-while run when the condition is false right away? How many loops does a break in nested loops leave? Which if does the else belong to? (true ? 1 : 2.5) / 2?
 * Key idea: A range-for with auto works on a COPY; to modify elements you need auto&. do-while runs at least once, while may run zero times. break/continue only affect the innermost loop. An else always binds to the nearest if without an else. Both branches of ?: are converted to a common type.
 * Intentional compiler warning: -Wdangling-else on the if (x) if (y) ... else block (gcc suggests adding braces).
 * Try next: Put a ; right after for (...) and see how many times the body runs. Add braces { } so the else belongs to the outer if.
 */
#include <iostream>
#include <vector>
using namespace std;

void print(const vector<int> &v)
{
    for (int x : v) cout << x << ' ';
    cout << '\n';
}

int main()
{
    vector<int> v{1, 2, 3};
    for (auto x : v) x *= 10;
    cout << "after for (auto x : v):  "; print(v);
    for (auto &x : v) x *= 10;
    cout << "after for (auto &x : v): "; print(v);

    int i = 10;
    do { cout << "do-while runs once\n"; } while (i < 5);
    while (i < 5) cout << "while never runs\n";

    cout << "continue/break: ";
    for (int k = 0; k < 10; ++k) {
        if (k % 2) continue;
        if (k > 6) break;
        cout << k << ' ';
    }
    cout << '\n';

    cout << "break in nested loops: ";
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) {
            if (c == 1) break;
            cout << r << c << ' ';
        }
    cout << '\n';

    int x = 1, y = 0;
    if (x)
        if (y)
            cout << "A\n";
    else
        cout << "the else belongs to if (y)\n";

    int score = 75;
    cout << "score 75: " << (score >= 50 ? "pass" : "fail") << '\n';
    cout << "(true ? 1 : 2.5) / 2 = " << (true ? 1 : 2.5) / 2 << '\n';

    int day = 3;
    switch (day) {
    case 1: case 7: cout << "weekend\n"; break;
    case 2: case 3: case 4: case 5: case 6: cout << "weekday\n"; break;
    default: cout << "invalid\n";
    }

    int sum = 0;
    for (int a = 1, b = 10; a < b; ++a, --b) sum += b - a;
    cout << "for with 2 variables: sum = " << sum << '\n';
    return 0;
}
