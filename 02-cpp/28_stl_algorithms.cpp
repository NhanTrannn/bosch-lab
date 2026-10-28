/*
 * Bài: STL: iterator, thuật toán, accumulate, remove không xoá, map/set, deque, stack/queue/priority_queue
 * Chủ đề: C++ STL (Udemy section 20)
 * Dự đoán trước khi chạy: accumulate(vd, 0) với vector<double>{1.5, 2.5} bằng mấy? Sau remove(…, 3), size còn bao nhiêu? set từ {5,1,3,1,5} in gì? map in theo thứ tự nào? priority_queue.top()? stack/queue lấy phần tử nào trước?
 * Ý chính: Giá trị khởi đầu quyết định kiểu của accumulate (0 → int → cắt phần thập phân). remove chỉ dồn phần tử, KHÔNG đổi size; phải erase. set/map luôn sắp xếp theo key và không trùng key. priority_queue mặc định là max-heap. stack LIFO, queue FIFO. end() trỏ sau phần tử cuối.
 * Thử thêm: Dùng std::greater<int> để priority_queue thành min-heap. Dùng multiset/multimap cho phép trùng key.
 */
#include <algorithm>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>
using namespace std;

template <typename C>
void show(const string &label, const C &c)
{
    cout << label;
    for (const auto &x : c) cout << x << ' ';
    cout << '\n';
}

int main()
{
    vector<double> vd{1.5, 2.5};
    cout << "accumulate(vd, 0) = " << accumulate(vd.begin(), vd.end(), 0) << ", accumulate(vd, 0.0) = " << accumulate(vd.begin(), vd.end(), 0.0) << '\n';

    vector<int> v{4, 3, 1, 3, 5, 3};
    auto it = find(v.begin(), v.end(), 5);
    cout << "find 5 o vi tri " << (it - v.begin()) << ", count 3 = " << count(v.begin(), v.end(), 3)
         << ", count_if chan = " << count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; }) << '\n';
    cout << "max = " << *max_element(v.begin(), v.end()) << ", find 9 == end ? " << (find(v.begin(), v.end(), 9) == v.end()) << '\n';

    auto new_end = remove(v.begin(), v.end(), 3);
    cout << "sau remove(3): size = " << v.size() << ", phan con lai = " << (new_end - v.begin()) << '\n';
    v.erase(new_end, v.end());
    show("sau erase: ", v);

    sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
    show("sort giam dan: ", v);
    transform(v.begin(), v.end(), v.begin(), [](int x) { return x * x; });
    show("transform x*x: ", v);
    reverse(v.begin(), v.end());
    show("reverse: ", v);

    set<int> s{5, 1, 3, 1, 5};
    show("set: ", s);
    auto [pos, inserted] = s.insert(3);
    cout << "insert 3 lan nua: inserted = " << inserted << ", *pos = " << *pos << '\n';

    map<string, int> m{{"Larry", 3}, {"Frank", 1}};
    m["Moe"] = 2;
    m["Frank"] += 10;
    cout << "map: ";
    for (const auto &[k, val] : m) cout << k << '=' << val << ' ';
    cout << ", size = " << m.size() << ", m.count(\"Joe\") = " << m.count("Joe") << '\n';

    deque<int> dq{2, 3};
    dq.push_front(1);
    dq.push_back(4);
    show("deque: ", dq);
    list<int> li{1, 2, 4};
    li.insert(next(li.begin(), 2), 3);
    show("list: ", li);

    stack<int> st;
    queue<int> q;
    priority_queue<int> pq;
    for (int x : {3, 1, 4, 1, 5}) { st.push(x); q.push(x); pq.push(x); }
    cout << "stack.top = " << st.top() << ", queue.front = " << q.front() << ", queue.back = " << q.back() << ", pq.top = " << pq.top() << '\n';
    cout << "pq lay ra: ";
    while (!pq.empty()) { cout << pq.top() << ' '; pq.pop(); }
    cout << '\n';
    return 0;
}
