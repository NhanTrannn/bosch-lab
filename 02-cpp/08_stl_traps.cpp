/*
 * Bài: Bẫy STL: map[], erase trong vòng lặp, iterator invalidation
 * Chủ đề: C++
 * Dự đoán trước khi chạy: size của map sau khi đọc m["b"]? Vector v còn gì? capacity trước/sau và p còn trỏ tới w[0] không?
 * Ý chính: map::operator[] TỰ CHÈN key nếu chưa có. vector::erase trả về iterator kế tiếp. push_back vượt capacity cấp phát lại → mọi pointer/iterator cũ không còn hợp lệ.
 * Thử thêm: Viết lại phần lọc số chẵn bằng erase-remove idiom: v.erase(remove_if(...), v.end()).
 */
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

int main()
{
    map<string, int> m;
    m["a"] = 1;
    cout << "size truoc = " << m.size() << '\n';
    if (m["b"] == 0)
        cout << "m[\"b\"] == 0\n";
    cout << "size sau [] = " << m.size() << '\n';
    cout << "count(\"c\") = " << m.count("c") << ", size = " << m.size() << '\n';

    vector<int> v = {1, 2, 3, 4, 5, 6};
    for (auto it = v.begin(); it != v.end();) {
        if (*it % 2 == 0)
            it = v.erase(it);
        else
            ++it;
    }
    for (int x : v)
        cout << x << ' ';
    cout << "| size = " << v.size() << '\n';

    vector<int> w;
    w.reserve(2);
    w.push_back(10);
    int *p = &w[0];
    cout << "capacity = " << w.capacity() << '\n';
    w.push_back(20);
    w.push_back(30);
    cout << "capacity = " << w.capacity() << '\n';
    cout << "p == &w[0] ? " << (p == &w[0] ? "yes" : "no") << '\n';
}
