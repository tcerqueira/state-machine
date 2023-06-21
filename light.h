#include "State.h"

struct FlipUp : public fsm::Event {};
struct FlipDown : public fsm::Event {};
struct UnusedEvent : public fsm::Event {};

class Light : public fsm::State<Light> {
public:
    using fsm::State<Light>::enter;
    using fsm::State<Light>::handleEvent;
    
    virtual void enter(const FlipUp& e);
    virtual std::unique_ptr<Light> handleEvent(const FlipUp& e);
    virtual std::unique_ptr<Light> handleEvent(const FlipDown& e);
};

class LightOn : public Light {
public:
    LightOn(int intensity) : _intensity(intensity) {}
    void enter(const FlipUp& e) override;
    std::unique_ptr<Light> handleEvent(const FlipDown& e) override;
private:
    int _intensity;
};

class LightOff : public Light {
public:
    std::unique_ptr<Light> handleEvent(const FlipUp& e) override;
};
