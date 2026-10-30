/*
 * Lab: STL: iterators, algorithms, accumulate, remove does not erase, map/set, deque, stack/queue/priority_queue
 * Topic: C++ STL (Udemy section 20)
 * Predict before running: What is accumulate(vd, 0) for vector<double>{1.5, 2.5}? After remove(…, 3), what is the size? What does a set built from {5,1,3,1,5} print? In what order does a map print? priority_queue.top()? Which element do stack/queue give first?
 * Key idea: The initial value decides accumulate's type (0 → int → fractions are cut). remove only shifts elements and does NOT change the size; you must erase. set/map are always sorted by key with no duplicate keys. priority_queue is a max-heap by default. stack is LIFO, queue FIFO. end() points past the last element.
 * Try next: Use std::greater<int> to make the priority_queue a min-heap. Use multiset/multimap to allow duplicate keys.
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
    cout << "find 5 at index " << (it - v.begin()) << ", count 3 = " << count(v.begin(), v.end(), 3)
         << ", count_if even = " << count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; }) << '\n';
    cout << "max = " << *max_element(v.begin(), v.end()) << ", find 9 == end ? " << (find(v.begin(), v.end(), 9) == v.end()) << '\n';

    auto new_end = remove(v.begin(), v.end(), 3);
    cout << "after remove(3): size = " << v.size() << ", kept elements = " << (new_end - v.begin()) << '\n';
    v.erase(new_end, v.end());
    show("after erase: ", v);

    sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
    show("sort descending: ", v);
    transform(v.begin(), v.end(), v.begin(), [](int x) { return x * x; });
    show("transform x*x: ", v);
    reverse(v.begin(), v.end());
    show("reverse: ", v);

    set<int> s{5, 1, 3, 1, 5};
    show("set: ", s);
    auto [pos, inserted] = s.insert(3);
    cout << "insert 3 again: inserted = " << inserted << ", *pos = " << *pos << '\n';

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
    cout << "pq pops: ";
    while (!pq.empty()) { cout << pq.top() << ' '; pq.pop(); }
    cout << '\n';
    return 0;
}
