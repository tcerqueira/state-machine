#include <iostream>
#include <type_traits>

/**
*   Util to get base class type.
*   Unimplemented to make sure it's only used in unevaluated contexts (sizeof, decltype, alignof)
*/
template<class T, class R>
T base_of(R T::*);

/**
*   Util to check if given type has a method with the given siganture.
*   Primary template with a static assertion for a meaningful error message if it ever gets instantiated.
*   We could leave it undefined if we didn't care.
*/
template<typename, typename T>
struct has_handle_event {
    static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
};

template<typename C, typename Ret, typename... Args>
struct has_handle_event<C, Ret(Args...)> {
private:
    template<typename T>
    static constexpr auto check(T*) -> typename std::is_same<
            decltype(std::declval<T>().handle_event(std::declval<Args>()...)),
            Ret    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        >::type;  // attempt to call it and see if the return type is correct

    template<typename>
    static constexpr std::false_type check(...);

    typedef decltype(check<C>(0)) type;

public:
    static constexpr bool value = type::value;
};

template<typename, typename T>
struct has_enter {
    static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
};

template<typename C, typename Ret, typename... Args>
struct has_enter<C, Ret(Args...)> {
private:
    template<typename T>
    static constexpr auto check(T*) -> typename std::is_same<
            decltype(std::declval<T>().enter(std::declval<Args>()...)),
            Ret    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        >::type;  // attempt to call it and see if the return type is correct

    template<typename>
    static constexpr std::false_type check(...);

    typedef decltype(check<C>(0)) type;

public:
    static constexpr bool value = type::value;
};

/**
*   Base class that every event declared event must derive from.
*   Usage:
*   
*   struct FlipSwitchUp : Event {};
*   struct FlipSwitchDown : Event {};
*/
struct Event {};

/**
*   Base class that every declared state model must derive from.
*   Usage:
*   
*   class Light : public State<Light> {
*   public:
*       void enter(const Event& e);
*       virtual void enter(const FlipUp& e) {};
*       // Light* handle_event(const Event& e) { return nullptr; };
*       virtual Light* handle_event(const FlipSwitchUp& e);
*       virtual Light* handle_event(const FlipSwitchDown& e);
*   };
*/
template<typename T>
struct State {
    // template<typename TEvent>
    virtual void enter(const Event& e) {
        std::cout << "State - generic entered new state\n";
    };

    // template<typename TEvent>
    virtual T* handle_event(const Event& e) {
        std::cout << "State - generic event\n";
        return nullptr;
    }

    void base_method() {}
};

template<typename TBaseState>
class StateMachine {
public:
    /**
    *   Assert that Event type is base of template state type.
    */
    static_assert(std::is_base_of<State<TBaseState>, TBaseState>::value, "Template parameter of state must derive from 'State'. Hint: 'BaseStateClass : public State {...}'");

    template<typename TInitState>
    StateMachine(TInitState *init_state)
        : _state{init_state} {
        /**
        *   Assert that state type is base of initial state type.
        */
        static_assert(std::is_base_of<TBaseState, TInitState>::value, "Template parameter of initial state must derive from type of state template parameter. Hint: 'StateClass : public BaseStateClass {...}'");
    }

    template<typename TEvent>
    void send_event(const TEvent& e) {
        /**
        *   Assert that Event type is base of template event type.
        */
        static_assert(std::is_base_of<Event, TEvent>::value, "Template parameter of event must derive from 'Event'.");

        /**
        *   Assert that state class implements 'handle_event' method for given event type.
        */
        static_assert(has_handle_event<TBaseState, TBaseState*(const TEvent&)>::value, "Base state class doesn't implement 'handle_event' for given event type.");
        // using BaseType = decltype(base_of(&TBaseState::base_method));
        // BaseType *base = (BaseType*)_state;
        // base->handle_event(e);
        // static_assert(has_handle_event<BaseType, BaseType*(const TEvent&)>::value, "Base state class doesn't implement 'handle_event' for given event type.");

        /**
        *   Assert that state class implements 'enter' method for given event type.
        */
        static_assert(has_enter<TBaseState, void(const TEvent&)>::value, "Base state class doesn't implement 'enter' for given event type.");

        handle_event(e);        
    }

    const TBaseState& current() const {
        return *_state;
    }

private:
    template<typename TEvent>
    void handle_event(const TEvent& e) {
        TBaseState *previous = _state;
        _state = _state->handle_event(e);
        // Change state
        if(_state != nullptr) {
            delete previous;
            _state->enter(e);
        }
        // Don't change state
        else {
            _state = previous;
        }
    }

private:
    TBaseState *_state;
};
