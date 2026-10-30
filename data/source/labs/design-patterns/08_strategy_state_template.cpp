/*
 * Lab: Strategy, State and Template Method
 * Topic: Design patterns — behavioral
 * Predict before running: What does the context compute with each strategy? Which states does the turnstile go through for coin, push, push, coin, coin? In what order does Game::play() call the steps for Cricket and for Football?
 * Key idea: Strategy encapsulates interchangeable algorithms behind one interface; the client picks one at run time (composition instead of if/else). State lets an object change its behavior when its internal state changes, by delegating to a state object that the states themselves replace. Template Method fixes the skeleton of an algorithm in a non-virtual base-class method and lets subclasses override individual steps (the "Hollywood principle": don't call us, we'll call you).
 * Try next: Pass the strategy as a std::function or a lambda instead of a class. Add a "broken" state to the turnstile.
 */
#include <iostream>
#include <memory>
#include <string>
using namespace std;

// ---- strategy
struct Operation {
    virtual ~Operation() = default;
    virtual int apply(int a, int b) const = 0;
    virtual string name() const = 0;
};
struct Add : Operation { int apply(int a, int b) const override { return a + b; } string name() const override { return "add"; } };
struct Subtract : Operation { int apply(int a, int b) const override { return a - b; } string name() const override { return "subtract"; } };
struct Multiply : Operation { int apply(int a, int b) const override { return a * b; } string name() const override { return "multiply"; } };
class Calculator {
public:
    void setStrategy(unique_ptr<Operation> op) { this->op = move(op); }
    void run(int a, int b) const { cout << "  " << op->name() << "(" << a << ", " << b << ") = " << op->apply(a, b) << '\n'; }
private:
    unique_ptr<Operation> op;
};

// ---- state
class Turnstile;
struct State {
    virtual ~State() = default;
    virtual void coin(Turnstile &t) = 0;
    virtual void push(Turnstile &t) = 0;
    virtual string name() const = 0;
};
class Turnstile {
public:
    Turnstile();
    void setState(unique_ptr<State> s) { state = move(s); }
    void coin() { state->coin(*this); }
    void push() { state->push(*this); }
    string stateName() const { return state->name(); }
private:
    unique_ptr<State> state;
};
struct Unlocked;
struct Locked : State {
    void coin(Turnstile &t) override;
    void push(Turnstile &) override { cout << "  push while locked: blocked\n"; }
    string name() const override { return "Locked"; }
};
struct Unlocked : State {
    void coin(Turnstile &) override { cout << "  coin while unlocked: refund\n"; }
    void push(Turnstile &t) override { cout << "  push: pass through, lock again\n"; t.setState(make_unique<Locked>()); }
    string name() const override { return "Unlocked"; }
};
void Locked::coin(Turnstile &t) { cout << "  coin: unlock\n"; t.setState(make_unique<Unlocked>()); }
Turnstile::Turnstile() : state(make_unique<Locked>()) {}

// ---- template method
class Game {
public:
    virtual ~Game() = default;
    void play()                        // the template method: fixed order, not virtual
    {
        initialize();
        start();
        end();
    }
protected:
    virtual void initialize() = 0;
    virtual void start() = 0;
    virtual void end() { cout << "  game over\n"; }   // a hook with a default
};
struct Cricket : Game {
    void initialize() override { cout << "  cricket initialized\n"; }
    void start() override { cout << "  cricket started\n"; }
    void end() override { cout << "  cricket finished\n"; }
};
struct Football : Game {
    void initialize() override { cout << "  football initialized\n"; }
    void start() override { cout << "  football started\n"; }
};

int main()
{
    cout << "Strategy:\n";
    Calculator calc;
    calc.setStrategy(make_unique<Add>());      calc.run(10, 5);
    calc.setStrategy(make_unique<Subtract>()); calc.run(10, 5);
    calc.setStrategy(make_unique<Multiply>()); calc.run(10, 5);

    cout << "State:\n";
    Turnstile t;
    cout << "  start: " << t.stateName() << '\n';
    t.coin(); t.push(); t.push(); t.coin(); t.coin();
    cout << "  end: " << t.stateName() << '\n';

    cout << "Template method:\n";
    Cricket().play();
    Football().play();
    return 0;
}
