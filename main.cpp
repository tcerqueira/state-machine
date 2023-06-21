// #include "state_machine.h"
#include "light.h"
#include "StateMachine.hpp"

int main() {
    auto fsm = fsm::StateMachine<Light>(std::make_unique<LightOn>(5));
    fsm.sendEvent(FlipDown());
    fsm.sendEvent(FlipUp());
    fsm.sendEvent(UnusedEvent());
    fsm.sendEvent(FlipDown());
    fsm.sendEvent(FlipDown());
    fsm.current();
}
