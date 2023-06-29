#include "StateMachine.hpp"
#include <iostream>

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

int main() {
    auto fsm = fsm::StateMachine<Light>(std::make_unique<LightOn>(5));
    fsm.sendEvent(FlipDown());
    fsm.sendEvent(FlipUp());
    fsm.sendEvent(UnusedEvent());
    fsm.sendEvent(FlipDown());
    fsm.sendEvent(FlipDown());
    fsm.current();
}

void Light::enter(const FlipUp& e) {
    std::cout << "Light - entered with FlipUp\n";
}

std::unique_ptr<Light> Light::handleEvent(const FlipUp& e) {
    std::cout << "Light - FlipUp event\n";
    return nullptr;
}

std::unique_ptr<Light> Light::handleEvent(const FlipDown& e) {
    std::cout << "Light - FlipDown event\n";
    return nullptr;
}

// #############################

void LightOn::enter(const FlipUp& e) {
    std::cout << "LightOn - entered with FlipUp\n";
}

std::unique_ptr<Light> LightOn::handleEvent(const FlipDown& e) {
    std::cout << "LightOn - FlipDown event\n";
    return std::make_unique<LightOff>();
}

std::unique_ptr<Light> LightOff::handleEvent(const FlipUp& e) {
    std::cout << "LightOff - FlipUp event\n";
    return std::make_unique<LightOn>(5);
}
