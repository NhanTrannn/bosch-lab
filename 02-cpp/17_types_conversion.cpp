/*
 * Lab: Variables, fundamental types, integer division and conversions
 * Topic: C++ basics (Udemy sections 6, 8)
 * Predict before running: 7/2, 7%2, -7/2, -7%2? What is avg = (1+2+4)/3? What does 'A'+1 print? unsigned 0 minus 1? 10/3*3 and 10*3/3?
 * Key idea: int / int is integer division (truncates toward zero); at least one double operand is needed for real division. Whether static_cast is applied BEFORE or AFTER the division changes the result. char is a number: 'A' + 1 becomes an int. {} (list init) blocks narrowing, = does not.
 * Try next: Uncomment int bad{3.99}; to see the narrowing error. Change avg to (1+2+4)/3.0.
 */
#include <iostream>
using namespace std;

int main()
{
    int a = 7, b = 2;
    cout << "7/2 = " << a / b << ", 7%2 = " << a % b << ", -7/2 = " << -7 / 2 << ", -7%2 = " << -7 % 2 << '\n';
    cout << "a/2.0 = " << a / 2.0 << ", static_cast<double>(a)/b = " << static_cast<double>(a) / b
         << ", static_cast<double>(a/b) = " << static_cast<double>(a / b) << '\n';

    double avg = (1 + 2 + 4) / 3;
    cout << "avg = " << avg << '\n';
    cout << "10/3*3 = " << 10 / 3 * 3 << ", 10*3/3 = " << 10 * 3 / 3 << '\n';

    int t = 3.99;                    // = allows truncation (no error)
    // int bad{3.99};                // {} reports a narrowing error
    cout << "int t = 3.99 -> " << t << ", (int)-3.99 = " << (int)-3.99 << '\n';

    char c = 'A';
    cout << "c + 1 = " << c + 1 << ", char(c + 1) = " << char(c + 1) << ", 'a' - 'A' = " << 'a' - 'A' << '\n';

    unsigned int u = 0;
    u--;
    cout << "unsigned 0 - 1 = " << u << '\n';

    bool flag = 42;
    cout << "bool flag = 42 -> " << flag << ", boolalpha: " << boolalpha << flag << noboolalpha << '\n';

    cout << "sizeof: char " << sizeof(char) << ", short " << sizeof(short) << ", int " << sizeof(int)
         << ", long " << sizeof(long) << ", long long " << sizeof(long long) << ", float " << sizeof(float)
         << ", double " << sizeof(double) << ", bool " << sizeof(bool) << '\n';

    const double PI = 3.14159;
    // PI = 3;                       // error: assigning to a const variable
    constexpr int SIZE = 2 * 5;     // computed at compile time
    int arr[SIZE] = {};
    cout << "PI = " << PI << ", SIZE = " << SIZE << ", sizeof(arr) = " << sizeof(arr) << '\n';
    return 0;
}
