/*
 * Bài: Operator overloading: + - == ++ (prefix/postfix) += [] << và hàm non-member
 * Chủ đề: C++ OOP (Udemy section 14)
 * Dự đoán trước khi chạy: c = a++ thì c và a là gì? Có bao nhiêu dòng [pre++] khi gọi a++? 2 * a gọi được không, a * 2 thì sao? (a += b) += b cho kết quả gì?
 * Ý chính: Postfix có tham số giả int, trả về bản copy CŨ (theo giá trị); prefix trả về *this theo tham chiếu → prefix rẻ hơn. Toán tử có vế trái không phải class của mình (2 * v, cout << v) phải là hàm non-member (thường là friend). Toán tử gán/gán kép trả về T& để nối chuỗi được.
 * Thử thêm: Viết thêm Vec operator*(const Vec&, int) để a * 2 chạy được. Không overload được: . .* :: ?: sizeof.
 */
#include <iostream>
using namespace std;

class Vec {
    int x, y;

public:
    Vec(int x = 0, int y = 0) : x{x}, y{y} {}

    Vec operator+(const Vec &o) const { return {x + o.x, y + o.y}; }
    Vec operator-() const { return {-x, -y}; }
    bool operator==(const Vec &o) const { return x == o.x && y == o.y; }
    bool operator!=(const Vec &o) const { return !(*this == o); }

    Vec &operator++()
    {
        ++x; ++y;
        cout << "[pre++] ";
        return *this;
    }
    Vec operator++(int)
    {
        Vec old = *this;
        ++*this;
        cout << "[post++] ";
        return old;
    }
    Vec &operator+=(const Vec &o)
    {
        x += o.x; y += o.y;
        return *this;
    }
    int &operator[](int i) { return i == 0 ? x : y; }

    friend ostream &operator<<(ostream &os, const Vec &v) { return os << '(' << v.x << ',' << v.y << ')'; }
    friend Vec operator*(int k, const Vec &v) { return {k * v.x, k * v.y}; }
};

int main()
{
    Vec a{1, 2}, b{3, 4};
    cout << "a + b = " << a + b << ", -a = " << -a << '\n';
    cout << "a == Vec{1,2} ? " << (a == Vec{1, 2}) << ", a != b ? " << (a != b) << '\n';

    Vec c = a++;
    cout << "\nc = a++: c = " << c << ", a = " << a << '\n';
    Vec d = ++a;
    cout << "\nd = ++a: d = " << d << ", a = " << a << '\n';

    cout << "2 * a = " << 2 * a << '\n';
    // cout << a * 2;   // lỗi: không có operator*(Vec, int)

    a[1] = 9;
    cout << "a[1] = 9 -> a = " << a << '\n';

    Vec e{1, 1};
    (e += b) += b;
    cout << "(e += b) += b -> e = " << e << '\n';

    Vec f = 5;   // constructor không explicit: 5 -> Vec(5, 0)
    cout << "Vec f = 5 -> " << f << ", f + 1 = " << f + 1 << '\n';
    return 0;
}
