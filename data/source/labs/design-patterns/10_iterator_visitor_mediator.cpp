/*
 * Lab: Iterator, Visitor and Mediator
 * Topic: Design patterns — behavioral
 * Predict before running: What does the range-for over NameRepository print, and the explicit hasNext()/next() loop? What do the two visitors print, and what is the total price? Who receives each chat message, and does a user receive their own message?
 * Key idea: Iterator gives sequential access to a collection's elements without exposing its internal representation (in C++: begin()/end() lets range-for work). Visitor separates an operation from the object structure it works on: each element calls accept(visitor) → visitor.visit(*this) (double dispatch), so a new operation is a new visitor class, without changing the elements. Mediator centralizes communication: objects talk to the mediator instead of to each other, turning many-to-many links into one-to-many.
 * Try next: Add a new element type to the visitor example: how many classes must change (the cost of Visitor)? Make the chat room send private messages.
 */
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ---- iterator
class NameRepository {
public:
    NameRepository(initializer_list<string> n) : names(n) {}
    // classic Java-style iterator
    class Iterator {
    public:
        explicit Iterator(const vector<string> &v) : v(v) {}
        bool hasNext() const { return i < v.size(); }
        const string &next() { return v[i++]; }
    private:
        const vector<string> &v;
        size_t i = 0;
    };
    Iterator iterator() const { return Iterator(names); }
    // C++-style iterators: enough for range-for
    auto begin() const { return names.begin(); }
    auto end() const { return names.end(); }
private:
    vector<string> names;
};

// ---- visitor
struct Keyboard; struct Mouse; struct Monitor;
struct PartVisitor {
    virtual ~PartVisitor() = default;
    virtual void visit(const Keyboard &) = 0;
    virtual void visit(const Mouse &) = 0;
    virtual void visit(const Monitor &) = 0;
};
struct Part {
    virtual ~Part() = default;
    virtual void accept(PartVisitor &v) const = 0;
};
struct Keyboard : Part { void accept(PartVisitor &v) const override { v.visit(*this); } int keys = 104; };
struct Mouse : Part { void accept(PartVisitor &v) const override { v.visit(*this); } };
struct Monitor : Part { void accept(PartVisitor &v) const override { v.visit(*this); } int inches = 27; };

struct DisplayVisitor : PartVisitor {
    void visit(const Keyboard &k) override { cout << "  displaying keyboard (" << k.keys << " keys)\n"; }
    void visit(const Mouse &) override { cout << "  displaying mouse\n"; }
    void visit(const Monitor &m) override { cout << "  displaying monitor (" << m.inches << "\")\n"; }
};
struct PriceVisitor : PartVisitor {
    int total = 0;
    void visit(const Keyboard &) override { total += 30; }
    void visit(const Mouse &) override { total += 15; }
    void visit(const Monitor &) override { total += 200; }
};

// ---- mediator
class User;
class ChatRoom {
public:
    void join(User *u) { users.push_back(u); }
    void send(const User &from, const string &msg) const;
private:
    vector<User *> users;
};
class User {
public:
    User(string name, ChatRoom &room) : name(move(name)), room(room) { room.join(this); }
    void say(const string &msg) const { room.send(*this, msg); }
    void receive(const User &from, const string &msg) const { cout << "  " << name << " <- " << from.name << ": " << msg << '\n'; }
private:
    string name;
    ChatRoom &room;
};
void ChatRoom::send(const User &from, const string &msg) const
{
    for (User *u : users)
        if (u != &from) u->receive(from, msg);
}

int main()
{
    cout << "Iterator:\n";
    NameRepository repo{"Robert", "John", "Julie", "Lora"};
    cout << "  range-for:";
    for (const string &n : repo) cout << ' ' << n;
    cout << "\n  hasNext/next:";
    for (auto it = repo.iterator(); it.hasNext();) cout << ' ' << it.next();
    cout << '\n';

    cout << "Visitor:\n";
    vector<unique_ptr<Part>> computer;
    computer.push_back(make_unique<Keyboard>());
    computer.push_back(make_unique<Mouse>());
    computer.push_back(make_unique<Monitor>());
    DisplayVisitor display;
    PriceVisitor price;
    for (auto &p : computer) { p->accept(display); p->accept(price); }
    cout << "  total price = " << price.total << '\n';

    cout << "Mediator:\n";
    ChatRoom room;
    User robert("Robert", room), john("John", room), julie("Julie", room);
    robert.say("Hi John!");
    john.say("Hello Robert!");
    return 0;
}
