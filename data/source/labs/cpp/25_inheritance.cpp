/*
 * Lab: Inheritance and polymorphism: calling the base constructor, protected, redefined (static binding) vs virtual (dynamic binding) functions
 * Topic: C++ OOP (Udemy sections 15, 16)
 * Predict before running: In what order are the constructors printed when creating a Trust? Through an Account&, which versions of withdraw (not virtual) and print (virtual) run? What does the loop over vector<Account*> print? In what order are destructors called on delete?
 * Key idea: Constructors run from the base class down to the derived class, destructors the other way. A NON-virtual function "redefined" in a derived class is chosen by the type of the pointer/reference (at compile time). A virtual function is chosen by the object's real type (at run time). A base with virtual functions needs a virtual destructor. A derived class calls the base version with Base::f().
 * Try next: Remove virtual from ~Account and see which destructor is lost when deleting through Account*. Add final to Savings and try to derive from it.
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Account {
protected:
    string name;
    double balance;

public:
    Account(string n, double b) : name{n}, balance{b} { cout << "  Account(" << name << ")\n"; }
    virtual ~Account() { cout << "  ~Account(" << name << ")\n"; }

    bool withdraw(double amount)
    {
        cout << "  Account::withdraw\n";
        if (amount > balance) return false;
        balance -= amount;
        return true;
    }
    virtual void print() const { cout << "  [Account " << name << ": " << balance << "]\n"; }
};

class Savings : public Account {
protected:
    double rate;

public:
    Savings(string n, double b, double r) : Account{n, b}, rate{r} { cout << "  Savings(" << name << ")\n"; }
    ~Savings() override { cout << "  ~Savings(" << name << ")\n"; }

    bool withdraw(double amount)
    {
        cout << "  Savings::withdraw\n";
        return Account::withdraw(amount);
    }
    void print() const override { cout << "  [Savings " << name << ": " << balance << ", " << rate << "%]\n"; }
};

class Trust : public Savings {
public:
    Trust(string n, double b) : Savings{n, b, 5} { cout << "  Trust(" << name << ")\n"; }
    ~Trust() override { cout << "  ~Trust(" << name << ")\n"; }
    void print() const override { cout << "  [Trust " << name << ": " << balance << "]\n"; }
};

class Shape {
public:
    virtual double area() const = 0;   // pure virtual → Shape is abstract
    virtual ~Shape() = default;
};
class Square : public Shape {
    double s;

public:
    Square(double s) : s{s} {}
    double area() const override { return s * s; }
};

int main()
{
    cout << "Create Trust t:\n";
    Trust t{"Tom", 1000};

    Account &ref = t;
    cout << "ref.withdraw(100):\n";
    ref.withdraw(100);
    cout << "t.withdraw(100):\n";
    t.withdraw(100);
    cout << "ref.print():\n";
    ref.print();

    cout << "Create vector<Account*>:\n";
    vector<Account *> accounts{new Account{"A", 10}, new Savings{"S", 20, 3}};
    cout << "Print through Account*:\n";
    for (const Account *acc : accounts) acc->print();
    cout << "delete:\n";
    for (Account *acc : accounts) delete acc;

    // Shape s;   // error: cannot create an object of an abstract class
    Square sq{3};
    const Shape &sh = sq;
    cout << "sh.area() = " << sh.area() << '\n';
    cout << "end of main:\n";
    return 0;
}
