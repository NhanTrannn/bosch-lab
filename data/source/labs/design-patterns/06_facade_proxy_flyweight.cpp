/*
 * Lab: Facade, Proxy and Flyweight
 * Topic: Design patterns — structural
 * Predict before running: How many lines does computer.start() print? When is the image actually loaded from disk, and how many times for two display() calls? How many Circle objects does the flyweight factory create for 10 circles in 3 colors?
 * Key idea: Facade gives a simple interface to a complex subsystem (the client calls one method instead of many). Proxy is a placeholder with the same interface that controls access to the real object (lazy loading, access control, caching, remote access). Flyweight shares objects to save memory: the intrinsic state (color) is shared and cached in a factory, the extrinsic state (position) is passed in by the client.
 * Try next: Make the proxy check a permission before loading. Count memory with and without the flyweight for 1 000 000 circles.
 */
#include <iostream>
#include <map>
#include <memory>
#include <string>
using namespace std;

// ---- facade
struct Cpu { void freeze() { cout << "  cpu: freeze\n"; } void jump(long a) { cout << "  cpu: jump to " << a << '\n'; } void execute() { cout << "  cpu: execute\n"; } };
struct Memory { void load(long a, const string &d) { cout << "  memory: load " << d << " at " << a << '\n'; } };
struct Disk { string read() { cout << "  disk: read boot sector\n"; return "bootloader"; } };
class ComputerFacade {
public:
    void start()
    {
        cpu.freeze();
        mem.load(0x7c00, disk.read());
        cpu.jump(0x7c00);
        cpu.execute();
    }
private:
    Cpu cpu; Memory mem; Disk disk;
};

// ---- proxy (virtual proxy: lazy loading)
struct Image {
    virtual ~Image() = default;
    virtual void display() = 0;
};
class RealImage : public Image {
public:
    explicit RealImage(string f) : file(move(f)) { cout << "  loading " << file << " from disk\n"; }
    void display() override { cout << "  displaying " << file << '\n'; }
private:
    string file;
};
class ProxyImage : public Image {
public:
    explicit ProxyImage(string f) : file(move(f)) {}
    void display() override
    {
        if (!real) real = make_unique<RealImage>(file);   // created only when really needed
        real->display();
    }
private:
    string file;
    unique_ptr<RealImage> real;
};

// ---- flyweight
class Circle {
public:
    explicit Circle(string color) : color(move(color)) { ++created; }
    void draw(int x, int y) const { (void)x; (void)y; }   // x, y are extrinsic state, passed in
    static int created;
private:
    string color;                                          // intrinsic, shared state
};
int Circle::created = 0;
struct CircleFactory {
    static shared_ptr<Circle> get(const string &color)
    {
        static map<string, shared_ptr<Circle>> cache;
        auto &c = cache[color];
        if (!c) c = make_shared<Circle>(color);
        return c;
    }
};

int main()
{
    cout << "Facade:\n";
    ComputerFacade computer;
    computer.start();

    cout << "Proxy:\n";
    ProxyImage img("photo.jpg");
    cout << "  proxy created, nothing loaded yet\n";
    img.display();
    img.display();

    cout << "Flyweight:\n";
    const char *colors[] = {"red", "green", "blue"};
    for (int i = 0; i < 10; ++i)
        CircleFactory::get(colors[i % 3])->draw(i * 10, i * 20);
    cout << "  10 circles drawn, Circle objects created = " << Circle::created << '\n';
    return 0;
}
