/*
 * Bài: unique_ptr, shared_ptr, weak_ptr
 * Chủ đề: C++
 * Dự đoán trước khi chạy: use_count in ra bao nhiêu ở mỗi chỗ? Node A, Node B có được huỷ không? Fixed X/Y thì sao, theo thứ tự nào?
 * Ý chính: unique_ptr chỉ move được, không copy. shared_ptr đếm tham chiếu; vòng shared_ptr trỏ nhau không bao giờ về 0 → leak. weak_ptr không tăng use_count, dùng để phá vòng.
 * Thử thêm: Đổi Node::next thành weak_ptr<Node> và chạy lại; thử lock() một weak_ptr sau khi object đã huỷ.
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
    cout << "u1 " << (u1 ? "co" : "rong") << ", *u2 = " << *u2 << '\n';

    shared_ptr<int> s1 = make_shared<int>(7);
    {
        shared_ptr<int> s2 = s1;
        auto s3 = s1;
        cout << "use_count trong block = " << s1.use_count() << '\n';
    }
    cout << "use_count sau block = " << s1.use_count() << '\n';

    {
        auto a = make_shared<Node>("A");
        auto b = make_shared<Node>("B");
        a->next = b;
        b->next = a;
        cout << "a.use_count = " << a.use_count() << '\n';
    }
    cout << "--- sau block Node ---\n";

    {
        auto x = make_shared<Fixed>("X");
        auto y = make_shared<Fixed>("Y");
        x->other = y;
        y->other = x;
        cout << "x.use_count = " << x.use_count() << '\n';
    }
    cout << "--- het ---\n";
}
