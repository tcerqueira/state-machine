#include "state_machine.h"

struct FlipUp : public Event {};
struct FlipDown : public Event {};

class Light : public State {
public:
    // const char* TAG() { return "Light"; }
    virtual void enter(const Event& e);
    virtual Light* handle_event(const Event& e);
    virtual Light* handle_event(const FlipUp& e);
    virtual Light* handle_event(const FlipDown& e);
};

class LightOn : public Light {
public:
    LightOn(int intensity) : _intensity(intensity) {}
    Light* handle_event(const FlipDown& e) override;
private:
    int _intensity;
};

class LightOff : public Light {
public:
    Light* handle_event(const FlipUp& e) override;
};
