/*
 * Lab: I/O streams: formatting with iomanip, sticky and non-sticky manipulators, stringstream, error state
 * Topic: C++ (Udemy section 19)
 * Predict before running: What does 1234.5678 print by default? setprecision(2) with and without fixed? How many outputs does setw apply to? Does hex stay in effect for the next output? What happens when "12 abc" is read into two ints?
 * Key idea: By default 6 SIGNIFICANT digits are printed (not 6 digits after the point). setprecision(n) without fixed = n significant digits, with fixed = n digits after the point. setw only applies to the NEXT output; the other manipulators (fixed, hex, left, setfill, boolalpha) stay until changed. A failed read puts the stream in the fail state and every later read is skipped until clear().
 * Try next: Write a file with ofstream and read it back with ifstream + getline. Check if (!file.is_open()).
 */
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

int main()
{
    double d = 1234.5678;
    cout << "default:             " << d << ", 1234567.89 -> " << 1234567.89 << '\n';
    cout << "setprecision(2):     " << setprecision(2) << d << '\n';
    cout << "fixed setprecision(2): " << fixed << d << '\n';
    cout << "still fixed:         " << 3.14159 << '\n';
    cout.unsetf(ios::fixed);
    cout << setprecision(6);

    cout << '[' << setw(6) << 42 << "][" << 42 << "]\n";
    cout << '[' << left << setw(6) << 42 << "][" << setw(6) << "ab" << "]\n";
    cout << right << setfill('*') << '[' << setw(6) << 42 << "][" << setw(3) << 7 << "]\n" << setfill(' ');

    cout << hex << 255 << ' ' << 16 << ' ' << showbase << 255 << dec << ' ' << 255 << noshowbase << '\n';
    cout << oct << 8 << dec << ' ' << boolalpha << (1 > 2) << noboolalpha << ' ' << showpoint << 2.0 << noshowpoint << ' ' << 2.0 << '\n';

    istringstream in{"Frank 100 1234.5"};
    string name;
    int num;
    double total;
    in >> name >> num >> total;
    cout << "read: " << name << " | " << num << " | " << total << '\n';

    istringstream bad{"12 abc 34"};
    int x = 0, y = 0, z = 0;
    bad >> x >> y >> z;
    cout << "\"12 abc 34\": x = " << x << ", y = " << y << ", z = " << z << ", fail = " << bad.fail() << '\n';
    bad.clear();
    string skip;
    bad >> skip >> z;
    cout << "after clear(): skip = " << skip << ", z = " << z << '\n';

    ostringstream out;
    out << "Total: " << setw(5) << 42 << '!';
    cout << "ostringstream: [" << out.str() << "]\n";

    int value;
    string input = "123abc";
    istringstream check{input};
    if (check >> value && check.eof()) cout << input << " is an integer\n";
    else cout << input << " is not an integer (read " << value << ")\n";
    return 0;
}
