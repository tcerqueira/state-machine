// #include "state_machine.h"
#include "light.h"

int main() {
    auto fsm = StateMachine<Light>(new LightOn(5));
    fsm.sendEvent(FlipDown());
    fsm.sendEvent(FlipUp());
    fsm.sendEvent(UnusedEvent());
    fsm.sendEvent(FlipDown());
    fsm.sendEvent(FlipDown());
    fsm.current();
}
