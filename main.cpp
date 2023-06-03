// #include "state_machine.h"
#include "light.h"

int main() {
    auto fsm = StateMachine<Light>(new LightOn(5));
    fsm.send_event(FlipDown());
    fsm.send_event(FlipUp());
    // fsm.send_event(UnusedEvent());
    fsm.current();
}
