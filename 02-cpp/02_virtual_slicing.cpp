/*
 * Bài: virtual, gọi qua object/pointer/reference và object slicing
 * Chủ đề: C++
 * Dự đoán trước khi chạy: Mỗi dòng in "Animal", "Woof" hay "name: ..."? sizeof(Animal) và sizeof(Dog)?
 * Ý chính: Dynamic dispatch chỉ xảy ra khi gọi hàm virtual qua pointer hoặc reference. Gán/truyền theo giá trị làm mất phần Derived (slicing).
 * Thử thêm: Bỏ chữ virtual trước speak() trong Animal và đoán lại toàn bộ.
 */
#include <iostream>
using namespace std;

struct Animal {
    virtual void speak() const { cout << "Animal\n"; }
    void name() const { cout << "name: Animal\n"; }
    virtual ~Animal() = default;
};

struct Dog : Animal {
    void speak() const override { cout << "Woof\n"; }
    void name() const { cout << "name: Dog\n"; }
};

void byValue(Animal a) { a.speak(); }
void byRef(const Animal &a) { a.speak(); }
void byPtr(const Animal *a) { a->speak(); }

int main()
{
    Dog d;
    Animal a = d;
    Animal &r = d;
    Animal *p = &d;

    a.speak();
    r.speak();
    p->speak();
    p->name();
    byValue(d);
    byRef(d);
    byPtr(&d);
    cout << sizeof(Animal) << ' ' << sizeof(Dog) << '\n';
}
