/*
 * Lab: STL traps: map[], erasing in a loop, iterator invalidation
 * Topic: C++
 * Predict before running: What is the map's size after reading m["b"]? What is left in vector v? What are the capacities before/after, and does p still point to w[0]?
 * Key idea: map::operator[] INSERTS the key if it is missing. vector::erase returns the next iterator. A push_back beyond capacity reallocates → every old pointer/iterator becomes invalid.
 * Try next: Rewrite the even-number filter with the erase-remove idiom: v.erase(remove_if(...), v.end()).
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
    cout << "size before = " << m.size() << '\n';
    if (m["b"] == 0)
        cout << "m[\"b\"] == 0\n";
    cout << "size after [] = " << m.size() << '\n';
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
