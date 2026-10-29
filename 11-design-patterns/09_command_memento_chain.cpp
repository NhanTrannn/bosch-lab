/*
 * Lab: Command (with undo), Memento and Chain of Responsibility
 * Topic: Design patterns — behavioral
 * Predict before running: What is the stock quantity after the queued orders run, and after undoing the last one? Which saved state does the editor return to after restore(0)? For each log level, which loggers in the chain write the message?
 * Key idea: Command turns a request into an object (execute/undo), so requests can be queued, logged and undone; the invoker does not know what the command does. Memento captures an object's internal state without exposing it, so it can be restored later (undo); the caretaker stores mementos but never looks inside. Chain of Responsibility passes a request along a chain of handlers until one (or several) handles it, decoupling the sender from the receivers.
 * Try next: Add redo with a second stack. Make the chain stop at the first handler that accepts the message.
 */
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ---- command
class Stock {
public:
    void buy(int n) { qty += n; cout << "  bought " << n << ", qty = " << qty << '\n'; }
    void sell(int n) { qty -= n; cout << "  sold " << n << ", qty = " << qty << '\n'; }
    int quantity() const { return qty; }
private:
    int qty = 10;
};
struct Order {
    virtual ~Order() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};
class BuyStock : public Order {
public:
    BuyStock(Stock &s, int n) : s(s), n(n) {}
    void execute() override { s.buy(n); }
    void undo() override { s.sell(n); }
private:
    Stock &s; int n;
};
class SellStock : public Order {
public:
    SellStock(Stock &s, int n) : s(s), n(n) {}
    void execute() override { s.sell(n); }
    void undo() override { s.buy(n); }
private:
    Stock &s; int n;
};
class Broker {                            // the invoker
public:
    void take(unique_ptr<Order> o) { queue.push_back(move(o)); }
    void placeAll() { for (auto &o : queue) { o->execute(); history.push_back(move(o)); } queue.clear(); }
    void undoLast() { if (history.empty()) return; cout << "  undo:\n"; history.back()->undo(); history.pop_back(); }
private:
    vector<unique_ptr<Order>> queue, history;
};

// ---- memento
class Editor {
public:
    class Memento {                        // opaque to everyone but Editor
        friend class Editor;
        explicit Memento(string t) : text(move(t)) {}
        string text;
    };
    void type(const string &s) { text += s; }
    Memento save() const { return Memento(text); }
    void restore(const Memento &m) { text = m.text; }
    const string &content() const { return text; }
private:
    string text;
};

// ---- chain of responsibility
enum Level { INFO = 1, DEBUG = 2, ERROR = 3 };
class Logger {
public:
    explicit Logger(Level l) : level(l) {}
    virtual ~Logger() = default;
    Logger *setNext(unique_ptr<Logger> n) { next = move(n); return next.get(); }
    void log(Level l, const string &msg) const
    {
        if (l >= level) write(msg);
        if (next) next->log(l, msg);
    }
protected:
    virtual void write(const string &msg) const = 0;
private:
    Level level;
    unique_ptr<Logger> next;
};
struct ConsoleLogger : Logger { using Logger::Logger; void write(const string &m) const override { cout << "    console: " << m << '\n'; } };
struct FileLogger : Logger { using Logger::Logger; void write(const string &m) const override { cout << "    file:    " << m << '\n'; } };
struct ErrorLogger : Logger { using Logger::Logger; void write(const string &m) const override { cout << "    error:   " << m << '\n'; } };

int main()
{
    cout << "Command:\n";
    Stock abc;
    Broker broker;
    broker.take(make_unique<BuyStock>(abc, 5));
    broker.take(make_unique<SellStock>(abc, 3));
    cout << "  2 orders queued, qty still " << abc.quantity() << '\n';
    broker.placeAll();
    broker.undoLast();
    cout << "  final qty = " << abc.quantity() << '\n';

    cout << "Memento:\n";
    Editor ed;
    vector<Editor::Memento> saved;
    ed.type("State #1"); saved.push_back(ed.save());
    ed.type(", #2");     saved.push_back(ed.save());
    ed.type(", #3");
    cout << "  current:  " << ed.content() << '\n';
    ed.restore(saved[0]);
    cout << "  restore(0): " << ed.content() << '\n';
    ed.restore(saved[1]);
    cout << "  restore(1): " << ed.content() << '\n';

    cout << "Chain of responsibility:\n";
    auto chain = make_unique<ErrorLogger>(ERROR);
    chain->setNext(make_unique<FileLogger>(DEBUG))->setNext(make_unique<ConsoleLogger>(INFO));
    cout << "  INFO message:\n";  chain->log(INFO, "this is information");
    cout << "  DEBUG message:\n"; chain->log(DEBUG, "this is debug info");
    cout << "  ERROR message:\n"; chain->log(ERROR, "this is an error");
    return 0;
}
