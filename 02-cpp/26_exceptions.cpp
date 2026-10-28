/*
 * Lab: Exceptions: throwing any type, catch order, catch(...), rethrow, custom exception classes
 * Topic: C++ (Udemy section 18)
 * Predict before running: Which catch block handles each test(k) call? For which k is the "after test(k)" line printed? When catching by value (catch(std::exception e)), what does what() return? What do stoi("abc") and v.at(10) throw?
 * Key idea: catch blocks are tried in the order written and the first match wins (not the "best" match) → derived classes must come before base classes. catch(...) catches everything. throw; (no expression) rethrows the exception being handled. Always catch by const reference to avoid slicing. Integer division by zero is NOT an exception (it is UB).
 * Intentional compiler warning: -Wcatch-value on catch (exception e) (catching by value → slicing).
 * Try next: Move catch (const std::exception&) above catch (const InsufficientFunds&) and read gcc's warning. Throw an exception from a noexcept function.
 */
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

class InsufficientFunds : public runtime_error {
public:
    double need;
    InsufficientFunds(double n) : runtime_error{"Insufficient funds"}, need{n} {}
};

void test(int k)
{
    if (k == 0) throw 42;
    if (k == 1) throw string{"an error as a string"};
    if (k == 2) throw InsufficientFunds{50};
    if (k == 3) throw invalid_argument{"bad argument"};
    if (k == 4) throw 3.14;
    cout << "  k = " << k << ": no throw\n";
}

void rethrow()
{
    try {
        throw logic_error{"from rethrow"};
    } catch (const exception &e) {
        cout << "  rethrow(): caught \"" << e.what() << "\", rethrowing\n";
        throw;
    }
}

int main()
{
    for (int k = 0; k <= 5; ++k) {
        try {
            test(k);
            cout << "  after test(" << k << ")\n";
        } catch (int e) {
            cout << "  catch int " << e << '\n';
        } catch (const string &e) {
            cout << "  catch string: " << e << '\n';
        } catch (const InsufficientFunds &e) {
            cout << "  catch InsufficientFunds: " << e.what() << ", need = " << e.need << '\n';
        } catch (const exception &e) {
            cout << "  catch exception: " << e.what() << '\n';
        } catch (...) {
            cout << "  catch (...)\n";
        }
    }

    try {
        rethrow();
    } catch (const logic_error &e) {
        cout << "  main caught it again: " << e.what() << '\n';
    }

    try {
        throw InsufficientFunds{10};
    } catch (exception e) {   // caught by value → slicing
        cout << "  caught by value: what() = " << e.what() << '\n';
    }

    try {
        stoi("abc");
    } catch (const invalid_argument &e) {
        cout << "  stoi(\"abc\") -> invalid_argument\n";
    }
    try {
        vector<int> v(3);
        v.at(10) = 1;
    } catch (const out_of_range &e) {
        cout << "  v.at(10) -> out_of_range\n";
    }
    return 0;
}
