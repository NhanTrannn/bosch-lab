/*
 * Bài: Kế thừa và đa hình: gọi constructor lớp cha, protected, hàm redefine (static binding) và virtual (dynamic binding)
 * Chủ đề: C++ OOP (Udemy section 15, 16)
 * Dự đoán trước khi chạy: Tạo Trust in ra các constructor theo thứ tự nào? Qua Account& gọi withdraw (không virtual) và print (virtual) chạy bản nào? Vòng lặp qua vector<Account*> in gì? Thứ tự destructor khi delete?
 * Ý chính: Constructor chạy từ lớp cha xuống lớp con, destructor ngược lại. Hàm KHÔNG virtual bị "redefine" ở lớp con được chọn theo kiểu của con trỏ/tham chiếu (lúc biên dịch). Hàm virtual chọn theo kiểu thật của object (lúc chạy). Base có hàm virtual thì destructor phải virtual. Lớp con gọi bản của lớp cha bằng Base::f().
 * Thử thêm: Bỏ virtual ở ~Account và xem destructor nào bị mất khi delete qua Account*. Thêm final vào Savings rồi thử kế thừa từ nó.
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
    virtual double area() const = 0;   // pure virtual → Shape là abstract
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
    cout << "Tao Trust t:\n";
    Trust t{"Tom", 1000};

    Account &ref = t;
    cout << "ref.withdraw(100):\n";
    ref.withdraw(100);
    cout << "t.withdraw(100):\n";
    t.withdraw(100);
    cout << "ref.print():\n";
    ref.print();

    cout << "Tao vector<Account*>:\n";
    vector<Account *> accounts{new Account{"A", 10}, new Savings{"S", 20, 3}};
    cout << "In qua Account*:\n";
    for (const Account *acc : accounts) acc->print();
    cout << "delete:\n";
    for (Account *acc : accounts) delete acc;

    // Shape s;   // lỗi: không tạo được object của lớp abstract
    Square sq{3};
    const Shape &sh = sq;
    cout << "sh.area() = " << sh.area() << '\n';
    cout << "het main:\n";
    return 0;
}
