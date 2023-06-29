/**
 * #################################### USAGE ####################################
 * ###############################################################################
 */

/**
 *   Usage:
 *
 *   struct FlipSwitchUp : fsm::Event {};
 *   struct FlipSwitchDown : fsm::Event {};
 *   strcut RotaryIntensity : fsm::Event {
 *       int mIntensity;
 *   };
 */

/**
 *   Usage:
 *
 *   class Light : public fsm::State<Light> {
 *   public:
 *       // Provides defaults to catch all undeclared events in class (optional).
 *       using fsm::State<Light>::enter;
 *       using fsm::State<Light>::handleEvent;
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
 *       virtual std::unique_ptr<Light> handleEvent(const FlipSwitchUp& e) {return nullptr;}
 *       virtual std::unique_ptr<Light> handleEvent(const FlipSwitchDown& e) {return nullptr;}
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
 *       std::unique_ptr<Light> handle_event(const FlipUp& e) override;
 *   };
 *
 */

#include <memory>
#include <type_traits>
#include "Event.h"
#include "State.h"

namespace fsm {

/**
 *   Util to get base class type.
 *   Unimplemented to make sure it's only used in unevaluated contexts (sizeof, decltype, alignof)
 */
template <class T, class R>
T base_of(R T::*);

/**
 *   Util to check if given type has a method with the given siganture.
 *   Primary template with a static assertion for a meaningful error message if it ever gets instantiated.
 *   We could leave it undefined if we didn't care.
 */
template <typename, typename T>
struct has_handleEvent {
    static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
};

template <typename C, typename Ret, typename... Args>
struct has_handleEvent<C, Ret(Args...)> {
  private:
    template <typename T>
    static constexpr auto check(T*) ->
            typename std::is_same<decltype(std::declval<T>().handleEvent(std::declval<Args>()...)),
                                  Ret>::type;  // attempt to call it and see if the return type is correct

    template <typename>
    static constexpr std::false_type check(...);

    typedef decltype(check<C>(0)) type;

  public:
    static constexpr bool value = type::value;
};

template <typename, typename T>
struct has_enter {
    static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
};

template <typename C, typename Ret, typename... Args>
struct has_enter<C, Ret(Args...)> {
  private:
    template <typename T>
    static constexpr auto check(T*) ->
            typename std::is_same<decltype(std::declval<T>().enter(std::declval<Args>()...)),
                                  Ret>::type;  // attempt to call it and see if the return type is correct

    template <typename>
    static constexpr std::false_type check(...);

    typedef decltype(check<C>(0)) type;

  public:
    static constexpr bool value = type::value;
};

template <typename TBaseState>
class StateMachine {
  public:
    /**
     *   Assert that Event type is base of template state type.
     */
    static_assert(std::is_base_of<State<TBaseState>, TBaseState>::value,
                  "Template parameter of state must derive from 'State'. Hint: 'BaseStateClass : public State {...}'");

    template <typename TInitState>
    explicit StateMachine(std::unique_ptr<TInitState>&& initState) : mState{std::move(initState)} {
        /**
         *   Assert that state type is base of initial state type.
         */
        static_assert(std::is_base_of<TBaseState, TInitState>::value,
                      "Template parameter of initial state must derive from type of state template parameter. Hint: "
                      "'StateClass : public BaseStateClass {...}'");
    }

    /**
     * @brief Emits the given event for the state machine to handle.
     *
     * @tparam TEvent Type of event to emit.
     * @param evt Event to emit.
     */
    template <typename TEvent>
    void sendEvent(const TEvent& evt) {
        /**
         *   Assert that Event type is base of template event type.
         */
        static_assert(std::is_base_of<Event, TEvent>::value, "Template parameter of event must derive from 'Event'.");

        /**
         *   Assert that state class implements 'handleEvent' method for given event type.
         */
        static_assert(has_handleEvent<TBaseState, std::unique_ptr<TBaseState>(const TEvent&)>::value,
                      "Base state class doesn't implement 'handleEvent' for given event type.");

        /**
         *   Assert that state class implements 'enter' method for given event type.
         */
        static_assert(has_enter<TBaseState, void(const TEvent&)>::value,
                      "Base state class doesn't implement 'enter' for given event type.");

        handleEvent(evt);
    }

    /**
     * @brief Retrieves the current state of the state machine. You cannot mutate it.
     *
     * @return Const reference to the current state.
     */
    const TBaseState& current() const { return *mState; }

  private:
    template <typename TEvent>
    void handleEvent(const TEvent& evt) {
        std::unique_ptr<TBaseState> nextState = mState->handleEvent(evt);
        if (nextState == nullptr) {  // Don't change state
            return;
        }

        mState->exit(evt);
        mState = std::move(nextState);
        mState->enter(evt);
    }

    std::unique_ptr<TBaseState> mState;
};

}  // namespace fsm
