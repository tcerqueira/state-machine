#include "state_machine.h"

struct FlipUp : public Event {};
struct FlipDown : public Event {};
struct UnusedEvent : public Event {};

class Light : public State<Light> {
public:
    using State<Light>::enter;
    using State<Light>::handleEvent;
    
    virtual void enter(const FlipUp& e);
    virtual Light* handleEvent(const FlipUp& e);
    virtual Light* handleEvent(const FlipDown& e);
};

class LightOn : public Light {
public:
    LightOn(int intensity) : _intensity(intensity) {}
    void enter(const FlipUp& e) override;
    Light* handleEvent(const FlipDown& e) override;
private:
    int _intensity;
};

class LightOff : public Light {
public:
    Light* handleEvent(const FlipUp& e) override;
};
