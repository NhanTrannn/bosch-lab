/*
 * Lab: Interpreter, Filter (Criteria) and Null Object
 * Topic: Design patterns — behavioral / structural
 * Predict before running: Which inputs do the rules "is male" (Robert OR John) and "is married woman" (Julie AND married) accept? Which persons does each criteria filter keep? What does the program print for the customers "Rob" and "Bob", and does it ever check for nullptr?
 * Key idea: Interpreter represents a small language's grammar as classes (terminal and non-terminal expressions) and evaluates sentences by calling interpret() recursively on the expression tree. Filter (Criteria) filters a set of objects with criteria objects that can be combined with And/Or, decoupling the filtering rules from the data. Null Object replaces a null reference with an object that does nothing (or returns a neutral value), so callers need no null checks.
 * Try next: Add a NotExpression to the interpreter. Return nullptr instead of the NullCustomer and see where the program crashes.
 */
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ---- interpreter
struct Expression {
    virtual ~Expression() = default;
    virtual bool interpret(const string &context) const = 0;
};
struct Terminal : Expression {
    explicit Terminal(string d) : data(move(d)) {}
    bool interpret(const string &c) const override { return c.find(data) != string::npos; }
    string data;
};
struct Or : Expression {
    Or(shared_ptr<Expression> a, shared_ptr<Expression> b) : a(move(a)), b(move(b)) {}
    bool interpret(const string &c) const override { return a->interpret(c) || b->interpret(c); }
    shared_ptr<Expression> a, b;
};
struct And : Expression {
    And(shared_ptr<Expression> a, shared_ptr<Expression> b) : a(move(a)), b(move(b)) {}
    bool interpret(const string &c) const override { return a->interpret(c) && b->interpret(c); }
    shared_ptr<Expression> a, b;
};

// ---- filter / criteria
struct Person { string name, gender, status; };
using People = vector<Person>;
struct Criteria {
    virtual ~Criteria() = default;
    virtual People meet(const People &p) const = 0;
};
struct Field : Criteria {
    Field(string Person::*f, string v) : f(f), v(move(v)) {}
    People meet(const People &p) const override
    {
        People out;
        copy_if(p.begin(), p.end(), back_inserter(out), [this](const Person &x) { return x.*f == v; });
        return out;
    }
    string Person::*f;
    string v;
};
struct AndCriteria : Criteria {
    AndCriteria(shared_ptr<Criteria> a, shared_ptr<Criteria> b) : a(move(a)), b(move(b)) {}
    People meet(const People &p) const override { return b->meet(a->meet(p)); }
    shared_ptr<Criteria> a, b;
};
struct OrCriteria : Criteria {
    OrCriteria(shared_ptr<Criteria> a, shared_ptr<Criteria> b) : a(move(a)), b(move(b)) {}
    People meet(const People &p) const override
    {
        People out = a->meet(p);
        for (const Person &x : b->meet(p))
            if (none_of(out.begin(), out.end(), [&](const Person &y) { return y.name == x.name; })) out.push_back(x);
        return out;
    }
    shared_ptr<Criteria> a, b;
};
void show(const string &label, const People &p)
{
    cout << "  " << label << ":";
    for (auto &x : p) cout << ' ' << x.name;
    cout << '\n';
}

// ---- null object
struct Customer {
    virtual ~Customer() = default;
    virtual string name() const = 0;
    virtual bool isNil() const = 0;
};
struct RealCustomer : Customer {
    explicit RealCustomer(string n) : n(move(n)) {}
    string name() const override { return n; }
    bool isNil() const override { return false; }
    string n;
};
struct NullCustomer : Customer {
    string name() const override { return "Not available in the customer database"; }
    bool isNil() const override { return true; }
};
unique_ptr<Customer> findCustomer(const string &name)
{
    for (const char *n : {"Rob", "Joe", "Julie"})
        if (name == n) return make_unique<RealCustomer>(name);
    return make_unique<NullCustomer>();     // never nullptr
}

int main()
{
    cout << "Interpreter:\n";
    auto isMale = make_shared<Or>(make_shared<Terminal>("Robert"), make_shared<Terminal>("John"));
    auto isMarriedWoman = make_shared<And>(make_shared<Terminal>("Julie"), make_shared<Terminal>("Married"));
    cout << "  John is male? " << isMale->interpret("John") << '\n';
    cout << "  Julie is male? " << isMale->interpret("Julie") << '\n';
    cout << "  \"Married Julie\" is a married woman? " << isMarriedWoman->interpret("Married Julie") << '\n';
    cout << "  \"Julie\" is a married woman? " << isMarriedWoman->interpret("Julie") << '\n';

    cout << "Filter:\n";
    People people{{"Robert", "Male", "Single"}, {"John", "Male", "Married"}, {"Laura", "Female", "Married"},
                  {"Diana", "Female", "Single"}, {"Mike", "Male", "Single"}, {"Bobby", "Male", "Single"}};
    auto male = make_shared<Field>(&Person::gender, "Male");
    auto female = make_shared<Field>(&Person::gender, "Female");
    auto single = make_shared<Field>(&Person::status, "Single");
    show("males", male->meet(people));
    show("females", female->meet(people));
    show("single males", AndCriteria(single, male).meet(people));
    show("single or female", OrCriteria(single, female).meet(people));

    cout << "Null object:\n";
    for (const char *n : {"Rob", "Bob", "Julie", "Laura"})
        cout << "  " << n << " -> " << findCustomer(n)->name() << '\n';
    return 0;
}
