#pragma once
#include <memory>
#include "Event.h"

namespace fsm {

/**
 * @brief Base class that every declared state model must derive from.
 *
 * @tparam T Base state type.
 */
template <typename T>
struct State {
    State() = default;
    State(const State&) = default;
    State& operator=(const State&) = default;
    State(State&&) noexcept = default;
    State& operator=(State&&) noexcept = default;
    virtual ~State() = default;

    /**
     * @brief Method called when the state machine enters the state.
     *  Override this method to handle the entering of a state.
     *  Also provides a default implementation that can be brought to scope
     *  by declaring 'using State<Light>::enter;'.
     *
     * @param evt Event that was emmited to enter this state.
     */
    virtual void enter(const Event& /*evt*/) {}

    /**
     * @brief Method called in the current state when an event is emitted to
     *  the state machine.
     *  Override this method to handle the event and opt to change the state
     *  by returning the new state or nullptr to stay in the same state.
     *  Also provides a default implementation that can be brought to scope
     *  by declaring 'using State<Light>::handleEvent;'.
     *
     * @param evt Emitted event to be handled.
     * @return Return the new state or nullptr to not change.
     */
    virtual std::unique_ptr<T> handleEvent(const Event& /*evt*/) { return nullptr; }

    // void base_state_method_dont_hide() {}
};

}  // namespace fsm
