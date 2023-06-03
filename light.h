#include "state_machine.h"

struct FlipUp : public Event {};
struct FlipDown : public Event {};
struct UnusedEvent : public Event {};

class Light : public State<Light> {
public:
    void enter(const Event& e);
    virtual void enter(const FlipUp& e);
    Light* handle_event(const Event& e);
    virtual Light* handle_event(const FlipUp& e);
    virtual Light* handle_event(const FlipDown& e);
};

class LightOn : public Light {
public:
    LightOn(int intensity) : _intensity(intensity) {}
    void enter(const FlipUp& e) override;
    Light* handle_event(const FlipDown& e) override;
private:
    int _intensity;
};

class LightOff : public Light {
public:
    Light* handle_event(const FlipUp& e) override;
};
