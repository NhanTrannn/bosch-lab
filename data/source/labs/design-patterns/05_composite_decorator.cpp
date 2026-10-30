/*
 * Lab: Composite and Decorator
 * Topic: Design patterns — structural
 * Predict before running: What is the total salary of the CEO's tree, and of the marketing head's subtree? In what order are the employees printed? What does each decorated coffee cost and print, and does the order of decorators change the description?
 * Key idea: Composite composes objects into tree structures so the client treats a single object (leaf) and a group (composite) uniformly through one interface, usually with recursion. Decorator wraps an object to add behavior at run time without changing its class; a decorator has the same interface as the wrapped object and forwards to it, so decorators can be stacked.
 * Try next: Add a remove() to the composite. Add a Whip decorator and compare the class count with a subclass for every combination (Milk+Sugar, Milk+Whip, ...).
 */
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ---- composite: employees and their subordinates
class Employee {
public:
    Employee(string name, string dept, int salary) : name(move(name)), dept(move(dept)), salary(salary) {}
    Employee &add(shared_ptr<Employee> e) { subs.push_back(move(e)); return *this; }
    int totalSalary() const
    {
        int t = salary;
        for (auto &e : subs) t += e->totalSalary();   // same call on leaves and groups
        return t;
    }
    void print(int depth = 0) const
    {
        cout << string(2 + depth * 2, ' ') << name << " (" << dept << ", " << salary << ")\n";
        for (auto &e : subs) e->print(depth + 1);
    }
private:
    string name, dept;
    int salary;
    vector<shared_ptr<Employee>> subs;
};

// ---- decorator: coffee with add-ons
struct Beverage {
    virtual ~Beverage() = default;
    virtual string description() const = 0;
    virtual double cost() const = 0;
};
struct Espresso : Beverage {
    string description() const override { return "Espresso"; }
    double cost() const override { return 2.0; }
};
class AddOn : public Beverage {          // the decorator base: same interface, wraps a Beverage
public:
    explicit AddOn(unique_ptr<Beverage> b) : inner(move(b)) {}
protected:
    unique_ptr<Beverage> inner;
};
struct Milk : AddOn {
    using AddOn::AddOn;
    string description() const override { return inner->description() + " + milk"; }
    double cost() const override { return inner->cost() + 0.5; }
};
struct Sugar : AddOn {
    using AddOn::AddOn;
    string description() const override { return inner->description() + " + sugar"; }
    double cost() const override { return inner->cost() + 0.25; }
};

int main()
{
    auto ceo = make_shared<Employee>("John", "CEO", 30000);
    auto sales = make_shared<Employee>("Robert", "Head Sales", 20000);
    auto marketing = make_shared<Employee>("Michel", "Head Marketing", 20000);
    sales->add(make_shared<Employee>("Richard", "Sales", 10000)).add(make_shared<Employee>("Rob", "Sales", 10000));
    marketing->add(make_shared<Employee>("Laura", "Marketing", 10000)).add(make_shared<Employee>("Bob", "Marketing", 10000));
    ceo->add(sales).add(marketing);

    cout << "Composite:\n";
    ceo->print();
    cout << "  total salary: CEO tree = " << ceo->totalSalary() << ", marketing subtree = " << marketing->totalSalary() << '\n';

    cout << "Decorator:\n";
    unique_ptr<Beverage> plain = make_unique<Espresso>();
    unique_ptr<Beverage> a = make_unique<Sugar>(make_unique<Milk>(make_unique<Espresso>()));
    unique_ptr<Beverage> b = make_unique<Milk>(make_unique<Milk>(make_unique<Sugar>(make_unique<Espresso>())));
    for (auto *x : {plain.get(), a.get(), b.get()})
        cout << "  " << x->description() << " = " << x->cost() << '\n';
    return 0;
}
