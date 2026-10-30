/*
 * Lab: Operators: ++/--, compound assignment, comparison, logical operators and short-circuit
 * Topic: C++ basics (Udemy section 8)
 * Predict before running: i, j, k after i++ and ++i? a after the chain of compound assignments? cnt after true || inc()? 0.1 + 0.2 == 0.3? 1 < 2 < 3 and 3 > 2 > 1? 5 / 2 * 2.0?
 * Key idea: i++ yields the OLD value, ++i yields the NEW value. && and || stop early (the right side may not run). && binds tighter than ||. Do not compare doubles with ==. a < b < c means (a < b) < c.
 * Intentional compiler warning: -Wparentheses on true || false && false (gcc suggests parentheses even though && already binds tighter) and on 1 < 2 < 3, 3 > 2 > 1 (gcc spots the trap).
 * Try next: Write if (x = 0) instead of if (x == 0) and look at the warning. Swap the two sides of the && on the division-by-n line.
 */
#include <cmath>
#include <iostream>
using namespace std;

int cnt = 0;
bool inc() { ++cnt; return true; }

int main()
{
    int i = 5;
    int j = i++;
    cout << "after j = i++: i = " << i << ", j = " << j << '\n';
    int k = ++i;
    cout << "after k = ++i: i = " << i << ", k = " << k << '\n';

    int a = 10;
    a += 5 * 2;      // 20
    a -= 3;          // 17
    a *= 2;          // 34
    a /= 3;          // 11
    a %= 7;          // 4
    cout << "a = " << a << '\n';
    int m = 3;
    m *= 2 + 1;      // m = m * (2 + 1)
    cout << "m *= 2 + 1 -> " << m << '\n';

    int n = 0;
    bool safe = (n != 0) && (10 / n > 1);
    cout << "(n != 0) && (10 / n > 1) = " << safe << " (no division by 0)\n";

    bool t1 = true || inc();
    bool t2 = false && inc();
    bool t3 = false || inc();
    cout << "t1 t2 t3 = " << t1 << t2 << t3 << ", cnt = " << cnt << '\n';

    cout << "true || false && false = " << (true || false && false) << '\n';
    cout << "!0 + 1 = " << !0 + 1 << ", !(0 + 1) = " << !(0 + 1) << '\n';

    cout << "0.1 + 0.2 == 0.3 ? " << (0.1 + 0.2 == 0.3) << ", |diff| < 1e-9 ? " << (fabs(0.1 + 0.2 - 0.3) < 1e-9) << '\n';
    cout << "1 < 2 < 3 = " << (1 < 2 < 3) << ", 3 > 2 > 1 = " << (3 > 2 > 1) << '\n';
    cout << "5 / 2 * 2.0 = " << 5 / 2 * 2.0 << ", 5 / 2.0 * 2 = " << 5 / 2.0 * 2 << '\n';
    cout << "2 + 3 * 4 = " << 2 + 3 * 4 << ", (2 + 3) * 4 = " << (2 + 3) * 4 << ", 10 - 4 - 3 = " << 10 - 4 - 3 << '\n';
    return 0;
}
