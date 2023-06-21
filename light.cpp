#include "light.h"
#include <iostream>

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
