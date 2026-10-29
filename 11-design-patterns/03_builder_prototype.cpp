/*
 * Lab: Builder and Prototype
 * Topic: Design patterns — creational
 * Predict before running: Which items and total cost does each meal have? What does the fluent HttpRequest builder produce? After cloning the prototype and changing the clone's tags, does the original change? With the shallow clone, does it?
 * Key idea: Builder constructs a complex object step by step and separates construction from representation (the same steps can build different meals; a fluent builder returns *this so calls chain). Prototype creates new objects by copying (cloning) an existing one through a virtual clone(), useful when creation is expensive or the concrete type is only known at run time. A clone must be a deep copy when the object owns resources.
 * Try next: Add a required field to HttpRequest and make build() throw if it is missing. Give ShallowDoc a std::vector instead of a pointer — is the copy still shallow?
 */
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ---- builder (meal example)
struct Item { string name; double price; };
class Meal {
public:
    void add(Item i) { items.push_back(i); }
    double cost() const { double c = 0; for (auto &i : items) c += i.price; return c; }
    void show() const { for (auto &i : items) cout << "  - " << i.name << " " << i.price << '\n'; cout << "  total " << cost() << '\n'; }
private:
    vector<Item> items;
};
struct MealBuilder {
    static Meal veg() { Meal m; m.add({"Veg burger", 25}); m.add({"Coke", 30}); return m; }
    static Meal nonVeg() { Meal m; m.add({"Chicken burger", 50.5}); m.add({"Pepsi", 35}); return m; }
};

// ---- fluent builder
struct HttpRequest {
    string method, url;
    map<string, string> headers;
    string body;
};
class RequestBuilder {
public:
    RequestBuilder &method(string m) { r.method = move(m); return *this; }
    RequestBuilder &url(string u) { r.url = move(u); return *this; }
    RequestBuilder &header(const string &k, const string &v) { r.headers[k] = v; return *this; }
    RequestBuilder &body(string b) { r.body = move(b); return *this; }
    HttpRequest build() const { return r; }
private:
    HttpRequest r{"GET", "/", {}, ""};
};

// ---- prototype
struct Document {
    virtual ~Document() = default;
    virtual unique_ptr<Document> clone() const = 0;
    virtual void show(const string &label) const = 0;
};
struct Report : Document {
    string title;
    vector<string> tags;
    Report(string t, vector<string> g) : title(move(t)), tags(move(g)) {}
    unique_ptr<Document> clone() const override { return make_unique<Report>(*this); }   // deep: vector copies its elements
    void show(const string &label) const override
    {
        cout << "  " << label << ": " << title << " [";
        for (auto &t : tags) cout << t << ' ';
        cout << "]\n";
    }
};
struct ShallowDoc {
    int *pages;   // owned through a raw pointer, copied as-is by the default copy constructor
};

int main()
{
    cout << "Veg meal:\n";
    MealBuilder::veg().show();
    cout << "Non-veg meal:\n";
    MealBuilder::nonVeg().show();

    HttpRequest req = RequestBuilder().method("POST").url("/api/runs").header("Content-Type", "application/json").body("{}").build();
    cout << "request: " << req.method << ' ' << req.url << ", " << req.headers.size() << " header(s), body " << req.body << '\n';

    Report original("Q3 report", {"draft"});
    unique_ptr<Document> copy = original.clone();
    static_cast<Report &>(*copy).tags.push_back("final");
    original.show("original");
    copy->show("clone   ");

    int n = 10;
    ShallowDoc a{&n};
    ShallowDoc b = a;
    *b.pages = 99;
    cout << "shallow copy: a.pages = " << *a.pages << ", same pointer? " << (a.pages == b.pages) << '\n';
    return 0;
}
