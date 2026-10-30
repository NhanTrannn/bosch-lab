/*
 * Lab: Factory, Factory Method and Abstract Factory
 * Topic: Design patterns — creational
 * Predict before running: What does ShapeFactory::create return for "SQUARE" and for "TRIANGLE"? Which button does each Dialog create? Which family of widgets does each GUI factory produce, and can a Windows button be mixed with a Linux checkbox?
 * Key idea: Factory (simple factory): one function decides which concrete class to create from a parameter, and the caller only sees the interface. Factory Method: a base class defines the algorithm and calls a virtual create...() that subclasses override. Abstract Factory: a "factory of factories" that creates a whole family of related objects, so products from one family always fit together.
 * Try next: Add a "Mac" family: how many classes must you add, and how many existing ones change (Open/Closed principle)? Make create() return a raw pointer and forget to delete it — run valgrind.
 */
#include <iostream>
#include <memory>
#include <string>
using namespace std;

// ---- simple factory
struct Shape {
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};
struct Circle : Shape { void draw() const override { cout << "  Circle::draw\n"; } };
struct Square : Shape { void draw() const override { cout << "  Square::draw\n"; } };

struct ShapeFactory {
    static unique_ptr<Shape> create(const string &type)
    {
        if (type == "CIRCLE") return make_unique<Circle>();
        if (type == "SQUARE") return make_unique<Square>();
        return nullptr;
    }
};

// ---- factory method
struct Button {
    virtual ~Button() = default;
    virtual string render() const = 0;
};
struct HtmlButton : Button { string render() const override { return "<button>OK</button>"; } };
struct WinButton : Button { string render() const override { return "[ OK ]"; } };

struct Dialog {
    virtual ~Dialog() = default;
    void show() const { cout << "  dialog shows " << createButton()->render() << '\n'; }   // uses the factory method
protected:
    virtual unique_ptr<Button> createButton() const = 0;
};
struct WebDialog : Dialog { unique_ptr<Button> createButton() const override { return make_unique<HtmlButton>(); } };
struct WinDialog : Dialog { unique_ptr<Button> createButton() const override { return make_unique<WinButton>(); } };

// ---- abstract factory
struct Checkbox {
    virtual ~Checkbox() = default;
    virtual string look() const = 0;
};
struct WinCheckbox : Checkbox { string look() const override { return "[x] (Windows)"; } };
struct GtkCheckbox : Checkbox { string look() const override { return "(x) (GTK)"; } };
struct GtkButton : Button { string render() const override { return "( OK ) (GTK)"; } };

struct GuiFactory {
    virtual ~GuiFactory() = default;
    virtual unique_ptr<Button> button() const = 0;
    virtual unique_ptr<Checkbox> checkbox() const = 0;
};
struct WinFactory : GuiFactory {
    unique_ptr<Button> button() const override { return make_unique<WinButton>(); }
    unique_ptr<Checkbox> checkbox() const override { return make_unique<WinCheckbox>(); }
};
struct LinuxFactory : GuiFactory {
    unique_ptr<Button> button() const override { return make_unique<GtkButton>(); }
    unique_ptr<Checkbox> checkbox() const override { return make_unique<GtkCheckbox>(); }
};

void buildUi(const GuiFactory &f)   // client code never names a concrete class
{
    cout << "  " << f.button()->render() << " + " << f.checkbox()->look() << '\n';
}

int main()
{
    cout << "Simple factory:\n";
    for (const char *t : {"CIRCLE", "SQUARE", "TRIANGLE"}) {
        auto s = ShapeFactory::create(t);
        if (s) s->draw();
        else cout << "  " << t << " -> nullptr\n";
    }

    cout << "Factory method:\n";
    WebDialog().show();
    WinDialog().show();

    cout << "Abstract factory:\n";
    buildUi(WinFactory{});
    buildUi(LinuxFactory{});
    return 0;
}
