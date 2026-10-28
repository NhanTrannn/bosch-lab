/*
 * Bài: Class: constructor overload, delegating constructor, copy constructor, const method, static member, shallow/deep copy
 * Chủ đề: C++ OOP (Udemy section 13)
 * Dự đoán trước khi chạy: Player a; in ra những dòng nào và theo thứ tự nào? display(c) gọi copy constructor không? Player::count ở các thời điểm? Thứ tự destructor khi ra khỏi khối? Shallow copy hai con trỏ có cùng địa chỉ không?
 * Ý chính: Delegating constructor chạy constructor đích (cả thân hàm) TRƯỚC, rồi mới tới thân của constructor ủy quyền. Truyền object theo giá trị gọi copy constructor. Object local huỷ theo thứ tự ngược với lúc tạo. Copy mặc định copy từng member (shallow): con trỏ bị copy địa chỉ → hai object cùng trỏ một vùng nhớ.
 * Thử thêm: Đổi display(Player p) thành display(const Player &p) và đếm lại số lần copy. Bỏ comment v.set_name(...) để thấy lỗi gọi hàm không const trên object const.
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
        cout << "count = " << Player::count << ", ra khoi khoi:\n";
    }
    cout << "count = " << Player::count << '\n';

    const Player v{"Villain"};
    cout << "v.get_name() = " << v.get_name() << '\n';
    // v.set_name("X");   // lỗi: set_name không phải const

    Shallow s1{1};
    Shallow s2 = s1;
    *s2.data = 42;
    cout << "Shallow: cung dia chi ? " << (s1.data == s2.data) << ", *s1.data = " << *s1.data << '\n';
    delete s1.data;      // chỉ delete một lần, delete thêm s2.data là double free

    Deep d1{1};
    Deep d2 = d1;
    *d2.data = 42;
    cout << "Deep: cung dia chi ? " << (d1.data == d2.data) << ", *d1.data = " << *d1.data << '\n';
    cout << "het main:\n";
    return 0;
}
