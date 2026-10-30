/*
 * Lab: Observer (publish/subscribe), with detaching and weak_ptr observers
 * Topic: Design patterns — behavioral
 * Predict before running: Which observers print after setState(15)? After detaching the octal observer and calling setState(10)? The log observer is held through a weak_ptr: what happens after it is destroyed?
 * Key idea: Observer defines a one-to-many dependency: when the subject's state changes, all registered observers are notified automatically. The subject only knows the observer interface, so they stay loosely coupled. Observers must be detached (or held as weak_ptr) before they die, otherwise the subject calls into a destroyed object (dangling pointer).
 * Try next: Detach an observer from inside its own update() while the subject is iterating — what goes wrong and how do you fix it? Replace the observer classes with std::function callbacks.
 */
#include <algorithm>
#include <bitset>
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

struct Observer {
    virtual ~Observer() = default;
    virtual void update(int state) = 0;
};

class Subject {
public:
    void attach(Observer *o) { observers.push_back(o); }
    void detach(Observer *o) { observers.erase(remove(observers.begin(), observers.end(), o), observers.end()); }
    void attachWeak(weak_ptr<Observer> o) { weak.push_back(move(o)); }
    void setState(int s)
    {
        state = s;
        cout << "setState(" << s << "):\n";
        for (Observer *o : observers) o->update(state);
        for (auto it = weak.begin(); it != weak.end();) {
            if (auto o = it->lock()) { o->update(state); ++it; }
            else { cout << "  (a weak observer is gone, removing it)\n"; it = weak.erase(it); }
        }
    }
private:
    int state = 0;
    vector<Observer *> observers;
    vector<weak_ptr<Observer>> weak;
};

struct BinaryObserver : Observer { void update(int s) override { cout << "  binary: " << bitset<8>(s) << '\n'; } };
struct OctalObserver : Observer { void update(int s) override { cout << "  octal:  " << oct << s << dec << '\n'; } };
struct HexObserver : Observer { void update(int s) override { cout << "  hex:    " << hex << uppercase << s << dec << '\n'; } };
struct LogObserver : Observer { void update(int s) override { cout << "  log:    state is now " << s << '\n'; } };

int main()
{
    Subject subject;
    BinaryObserver bin;
    OctalObserver oct_;
    HexObserver hex_;
    subject.attach(&bin);
    subject.attach(&oct_);
    subject.attach(&hex_);

    auto log = make_shared<LogObserver>();
    subject.attachWeak(log);

    subject.setState(15);
    subject.detach(&oct_);
    subject.setState(10);
    log.reset();
    subject.setState(3);
    return 0;
}
