#include <iostream>
#include <type_traits>

struct Event {};

struct State {};

/**
*   Util to get base class type.
*   Unimplemented to make sure it's only used in unevaluated contexts (sizeof, decltype, alignof)
*/
// template<class T, class R>
// T base_of(R T::*);

// template<typename InitState_T>
template<typename T>
class StateMachine {
private:
    // using State_T = decltype(base_of(&InitState_T::TAG));
    using State_T = T;
    /**
    *   Assert that Event type is base of template state type.
    */
    static_assert(std::is_base_of<State, State_T>::value, "Template parameter of state must derive from 'State'.");

public:
    // template<typename... Args>
    // StateMachine(Args&&... init_state_args)
    //     : _state{new InitState_T(std::forward<Args>(init_state_args))...} {
    //     /**
    //     *   Assert that forwarded arguments are valid for the type of initial state.
    //     */
    //     static_assert(std::is_constructible<InitState_T, Args&&...>::value, "No matching constructor for initial state type.");
    // }

    template<typename InitState_T>
    StateMachine(InitState_T *init_state)
        : _state{init_state} {
        /**
        *   Assert that state type is base of initial state type.
        */
        static_assert(std::is_base_of<State_T, InitState_T>::value, "Template parameter of initial state must derive from type of state template parameter.");
    }

    template<typename Event_T>
    void send_event(const Event_T& e) {
        /**
        *   Assert that Event type is base of template event type.
        */
        static_assert(std::is_base_of<Event, Event_T>::value, "Template parameter of event must derive from 'Event'.");

        handle_event(e);        
    }

    const State_T& current() const {
        return *_state;
    }

private:
    template<typename Event_T>
    void handle_event(const Event_T& e) {
        State_T *previous = _state;
        _state = _state->handle_event(e);
        delete previous;
        _state->enter(e);
    }

private:
    State_T *_state;
};
