/*
 * Lab: Adapter and Bridge
 * Topic: Design patterns — structural
 * Predict before running: Which files can AudioPlayer play directly, which through the adapter, and which not at all? What does each bridged shape print? How many classes would you need for 3 shapes × 3 renderers with plain inheritance, and with Bridge?
 * Key idea: Adapter converts the interface of an existing class into the interface the client expects, so incompatible classes can work together (object adapter = wrap an instance; class adapter = inherit from the adaptee). Bridge decouples an abstraction from its implementation so the two can vary independently: the abstraction holds a pointer to an implementation interface instead of inheriting from it.
 * Try next: Write a class adapter (inherit from both MediaPlayer and VlcPlayer). Add a "Blue" renderer to the bridge: how many classes change?
 */
#include <iostream>
#include <memory>
#include <string>
using namespace std;

// ---- adapter: the client knows MediaPlayer, the existing library has a different interface
struct MediaPlayer {
    virtual ~MediaPlayer() = default;
    virtual void play(const string &type, const string &file) = 0;
};
struct AdvancedMediaPlayer {              // existing, incompatible interface
    virtual ~AdvancedMediaPlayer() = default;
    virtual void playVlc(const string &file) = 0;
    virtual void playMp4(const string &file) = 0;
};
struct VlcPlayer : AdvancedMediaPlayer {
    void playVlc(const string &f) override { cout << "  vlc player: " << f << '\n'; }
    void playMp4(const string &) override {}
};
struct Mp4Player : AdvancedMediaPlayer {
    void playVlc(const string &) override {}
    void playMp4(const string &f) override { cout << "  mp4 player: " << f << '\n'; }
};
class MediaAdapter : public MediaPlayer {  // object adapter: wraps an AdvancedMediaPlayer
public:
    explicit MediaAdapter(const string &type)
        : adv(type == "vlc" ? unique_ptr<AdvancedMediaPlayer>(make_unique<VlcPlayer>()) : make_unique<Mp4Player>()) {}
    void play(const string &type, const string &file) override
    {
        if (type == "vlc") adv->playVlc(file);
        else adv->playMp4(file);
    }
private:
    unique_ptr<AdvancedMediaPlayer> adv;
};
struct AudioPlayer : MediaPlayer {
    void play(const string &type, const string &file) override
    {
        if (type == "mp3") cout << "  built-in mp3: " << file << '\n';
        else if (type == "vlc" || type == "mp4") MediaAdapter(type).play(type, file);
        else cout << "  invalid media: " << type << '\n';
    }
};

// ---- bridge: Shape (abstraction) holds a DrawAPI (implementation)
struct DrawAPI {
    virtual ~DrawAPI() = default;
    virtual void drawCircle(int r, int x, int y) const = 0;
};
struct RedCircle : DrawAPI { void drawCircle(int r, int x, int y) const override { cout << "  red circle r=" << r << " at (" << x << "," << y << ")\n"; } };
struct GreenCircle : DrawAPI { void drawCircle(int r, int x, int y) const override { cout << "  green circle r=" << r << " at (" << x << "," << y << ")\n"; } };

class Shape {
public:
    explicit Shape(shared_ptr<DrawAPI> api) : api(move(api)) {}
    virtual ~Shape() = default;
    virtual void draw() const = 0;
protected:
    shared_ptr<DrawAPI> api;
};
class Circle : public Shape {
public:
    Circle(int x, int y, int r, shared_ptr<DrawAPI> api) : Shape(move(api)), x(x), y(y), r(r) {}
    void draw() const override { api->drawCircle(r, x, y); }
private:
    int x, y, r;
};

int main()
{
    cout << "Adapter:\n";
    AudioPlayer player;
    player.play("mp3", "beyond the horizon.mp3");
    player.play("mp4", "alone.mp4");
    player.play("vlc", "far far away.vlc");
    player.play("avi", "mind me.avi");

    cout << "Bridge:\n";
    Circle(100, 100, 10, make_shared<RedCircle>()).draw();
    Circle(100, 100, 10, make_shared<GreenCircle>()).draw();
    return 0;
}
