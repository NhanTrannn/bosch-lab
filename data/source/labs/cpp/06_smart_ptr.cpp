/*
 * Lab: unique_ptr, shared_ptr, weak_ptr
 * Topic: C++
 * Predict before running: What does use_count print at each point? Are Node A and Node B destroyed? What about Fixed X/Y, and in which order?
 * Key idea: unique_ptr can only be moved, not copied. shared_ptr counts references; a cycle of shared_ptrs never reaches 0 → leak. weak_ptr does not increase use_count and is used to break cycles.
 * Try next: Change Node::next to weak_ptr<Node> and run again; try lock() on a weak_ptr after the object is gone.
 */
#include <iostream>
#include <memory>
#include <string>
using namespace std;

struct Node {
    string name;
    shared_ptr<Node> next;
    Node(string n) : name(n) { cout << "Node " << name << " ctor\n"; }
    ~Node() { cout << "Node " << name << " dtor\n"; }
};

struct Fixed {
    string name;
    weak_ptr<Fixed> other;
    Fixed(string n) : name(n) {}
    ~Fixed() { cout << "Fixed " << name << " dtor\n"; }
};

int main()
{
    unique_ptr<int> u1 = make_unique<int>(42);
    unique_ptr<int> u2 = move(u1);
    cout << "u1 " << (u1 ? "has a value" : "is empty") << ", *u2 = " << *u2 << '\n';

    shared_ptr<int> s1 = make_shared<int>(7);
    {
        shared_ptr<int> s2 = s1;
        auto s3 = s1;
        cout << "use_count inside block = " << s1.use_count() << '\n';
    }
    cout << "use_count after block = " << s1.use_count() << '\n';

    {
        auto a = make_shared<Node>("A");
        auto b = make_shared<Node>("B");
        a->next = b;
        b->next = a;
        cout << "a.use_count = " << a.use_count() << '\n';
    }
    cout << "--- after the Node block ---\n";

    {
        auto x = make_shared<Fixed>("X");
        auto y = make_shared<Fixed>("Y");
        x->other = y;
        y->other = x;
        cout << "x.use_count = " << x.use_count() << '\n';
    }
    cout << "--- end of main ---\n";
}
