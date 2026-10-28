/*
 * Lab: Classes: constructor overloading, delegating constructors, the copy constructor, const methods, static members, shallow/deep copy
 * Topic: C++ OOP (Udemy section 13)
 * Predict before running: Which lines does Player a; print, and in what order? Does display(c) call the copy constructor? What is Player::count at each point? In what order are destructors called when leaving the block? Do the two pointers of the shallow copy share an address?
 * Key idea: A delegating constructor runs the target constructor (including its body) FIRST, then the body of the delegating constructor. Passing an object by value calls the copy constructor. Local objects are destroyed in the reverse order of creation. The default copy copies member by member (shallow): a pointer's address is copied → both objects point to the same memory.
 * Try next: Change display(Player p) to display(const Player &p) and count the copies again. Uncomment v.set_name(...) to see the error of calling a non-const function on a const object.
 */
#include <iostream>
#include <string>
using namespace std;

class Player {
    string name;
    int health;
    int xp;

public:
    static int count;

    Player() : Player{"None", 0, 0} { cout << "  no-args\n"; }
    Player(string n) : Player{n, 100, 0} { cout << "  one-arg\n"; }
    Player(string n, int h, int x) : name{n}, health{h}, xp{x}
    {
        ++count;
        cout << "  three-args " << name << '\n';
    }
    Player(const Player &src) : Player{src.name, src.health, src.xp} { cout << "  copy " << name << '\n'; }
    ~Player()
    {
        --count;
        cout << "  ~" << name << '\n';
    }

    string get_name() const { return name; }
    void set_name(string n) { name = n; }
};
int Player::count = 0;

void display(Player p) { cout << "  display " << p.get_name() << '\n'; }

struct Shallow {
    int *data;
    Shallow(int v) : data{new int{v}} {}
};
struct Deep {
    int *data;
    Deep(int v) : data{new int{v}} {}
    Deep(const Deep &src) : data{new int{*src.data}} {}
    ~Deep() { delete data; }
};

int main()
{
    {
        cout << "Player a:\n";
        Player a;
        cout << "Player b{\"Frank\"}:\n";
        Player b{"Frank"};
        cout << "Player c{\"Hero\", 100, 5}:\n";
        Player c{"Hero", 100, 5};
        cout << "count = " << Player::count << '\n';
        cout << "display(c):\n";
        display(c);
        cout << "Player d = c:\n";
        Player d = c;
        d.set_name("Clone");
        cout << "count = " << Player::count << ", leaving the block:\n";
    }
    cout << "count = " << Player::count << '\n';

    const Player v{"Villain"};
    cout << "v.get_name() = " << v.get_name() << '\n';
    // v.set_name("X");   // error: set_name is not const

    Shallow s1{1};
    Shallow s2 = s1;
    *s2.data = 42;
    cout << "Shallow: same address ? " << (s1.data == s2.data) << ", *s1.data = " << *s1.data << '\n';
    delete s1.data;      // delete only once; deleting s2.data too would be a double free

    Deep d1{1};
    Deep d2 = d1;
    *d2.data = 42;
    cout << "Deep: same address ? " << (d1.data == d2.data) << ", *d1.data = " << *d1.data << '\n';
    cout << "end of main:\n";
    return 0;
}
