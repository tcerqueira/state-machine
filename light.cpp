#include "light.h"
#include <iostream>

void Light::enter(const Event& e) {
    std::cout << "Light - generic entered new state\n";
}

void Light::enter(const FlipUp& e) {
    std::cout << "Light - generic entered new state with FlipUp\n";
}

Light* Light::handle_event(const Event& e) {
    std::cout << "Light - generic event\n";
    return nullptr;
}

Light* Light::handle_event(const FlipUp& e) {
    std::cout << "Light - generic FlipUp event\n";
    return this;
}

Light* Light::handle_event(const FlipDown& e) {
    std::cout << "Light - generic FlipDown event\n";
    return this;
}

// #############################

void LightOn::enter(const FlipUp& e) {
    std::cout << "LightOn - entered with FlipUp\n";
}

Light* LightOn::handle_event(const FlipDown& e) {
    std::cout << "LightOn - FlipDown event\n";
    return new LightOff();
}

Light* LightOff::handle_event(const FlipUp& e) {
    std::cout << "LightOff - FlipUp event\n";
    return new LightOn(5);
}
