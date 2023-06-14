/**
 * @file
 * @copyright (C) 2022-2023 BMW AG
 */

/**
 * #################################### USAGE ####################################
 * ###############################################################################
 */

/**
 *   Usage:
 *   
 *   struct FlipSwitchUp : Event {};
 *   struct FlipSwitchDown : Event {};
 *   strcut RotaryIntensity : Event {
 *       int mIntensity;
 *   };
*/

/**
 *   Usage:
 *   
 *   class Light : public State<Light> {
 *   public:
 *       // Provides defaults to catch all undeclared events in class (optional).
 *       using State<Light>::enter;
 *       using State<Light>::handleEvent;
 *
 *       // Non-virtual so all states behave the same when transiting from the
 *       // given event.
 *       void enter(const RotaryIntensity& e) {}
 *
 *       // Declaration of the events that the states will handle.
 *       // Default implementation is required.
 *       virtual void enter(const FlipSwitchUp& e) {}
 *       virtual void enter(const FlipSwitchDown& e) {}
 *
 *       // Return the next state object. Base class takes ownership.
 *       // Return nullptr means 'Don't change state'.
 *       virtual Light* handleEvent(const FlipSwitchUp& e) {return nullptr;}
 *       virtual Light* handleEvent(const FlipSwitchDown& e) {return nullptr;}
 *   };
 */

/**
 * Usage:
 * 
 * class LightOn : public Light {
 *   public:
 *       LightOn(int intensity) : mIntensity(intensity) {}
 *       void enter(const FlipUp& e) override;
 *       Light* handle_event(const FlipDown& e) override;
 *   private:
 *       int mIntensity;
 *   };
 *   
 *   class LightOff : public Light {
 *   public:
 *       Light* handle_event(const FlipUp& e) override;
 *   };
 * 
 */
 
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
struct has_handleEvent {
    static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
};

template<typename C, typename Ret, typename... Args>
struct has_handleEvent<C, Ret(Args...)> {
private:
    template<typename T>
    static constexpr auto check(T*) -> typename std::is_same<
            decltype(std::declval<T>().handleEvent(std::declval<Args>()...)),
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

struct Event {
    virtual ~Event() = default;
};

/**
 * @brief Base class that every declared state model must derive from.
 * 
 * @tparam T Base state type.
 */
template<typename T>
struct State {
    virtual ~State() = default;

    /**
     * @brief Method called when the state machine enters the state.
     *  Override this method to handle the entering of a state.
     *  Also provides a default implementation that can be brought to scope
     *  by declaring 'using State<Light>::enter;'.
     * 
     * @param evt Event that was emmited to enter this state.
     */
    virtual void enter(const Event& evt) {}

    /**
     * @brief Method called in the current state when an event is emitted to
     *  the state machine.
     *  Override this method to handle the event and opt to change the state
     *  by returning the new state or nullptr to stay in the same state.
     *  If the same state is returned it will call the enter handler.
     *  Also provides a default implementation that can be brought to scope
     *  by declaring 'using State<Light>::handleEvent;'.
     * 
     * @param evt Emitted event to be handled.
     * @return T* Return the new state or nullptr to not change.
     */
    virtual T* handleEvent(const Event& evt) {
        return nullptr;
    }

    // void base_state_method_dont_override() {}
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
        : mState{init_state} {
        /**
        *   Assert that state type is base of initial state type.
        */
        static_assert(std::is_base_of<TBaseState, TInitState>::value, "Template parameter of initial state must derive from type of state template parameter. Hint: 'StateClass : public BaseStateClass {...}'");
    }

    ~StateMachine() {
        delete mState;
    }

    /**
     * @brief Emits the given event for the state machine to handle.
     * 
     * @tparam TEvent Type of event to emit.
     * @param evt Event to emit.
     */
    template<typename TEvent>
    void sendEvent(const TEvent& evt) {
        /**
        *   Assert that Event type is base of template event type.
        */
        static_assert(std::is_base_of<Event, TEvent>::value, "Template parameter of event must derive from 'Event'.");

        /**
        *   Assert that state class implements 'handleEvent' method for given event type.
        */
        static_assert(has_handleEvent<TBaseState, TBaseState*(const TEvent&)>::value, "Base state class doesn't implement 'handleEvent' for given event type.");

        /**
        *   Assert that state class implements 'enter' method for given event type.
        */
        static_assert(has_enter<TBaseState, void(const TEvent&)>::value, "Base state class doesn't implement 'enter' for given event type.");

        handleEvent(evt);        
    }

    /**
     * @brief Retrieves the current state of the state machine. You cannot mutate it.
     * 
     * @return const TBaseState& Const reference to the current state.
     */
    const TBaseState& current() const {
        return *mState;
    }

private:
    template<typename TEvent>
    void handleEvent(const TEvent& evt) {
        TBaseState *previous = mState;
        mState = mState->handleEvent(evt);
        
        if(mState != nullptr) { // Change state
            if(previous != mState)  // If it's the same state just retrigger enter
                delete previous;
            mState->enter(evt);
        }
        else {  // Don't change state
            mState = previous;
        }
    }

private:
    TBaseState *mState;
};
