/****************************************************************************
 *
 *      Copyright (c) 2022, Auterion Ltd. All rights reserved.
 *
 * All information contained herein is, and remains the property of
 * Auterion Ltd. and its suppliers, if any. The intellectual and technical
 * concepts contained herein are proprietary to Auterion Ltd. and its
 * suppliers and may be covered by U.S. and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.
 * Reproduction or distribution, in whole or in part, of this information
 * or reproduction of this material is strictly forbidden unless prior
 * written permission is obtained from Auterion Ltd.
 *
 ****************************************************************************/

/**
 * @author Julian Kent <julian@auterion.com>
 */

#pragma once

#include <mutex>

namespace usm {

enum Transition { T_REPEAT, T_NEXT1, T_NEXT2, T_NEXT3, T_NEXT4, T_ERROR };

template<typename StateEnum>
class StateMachine {
public:
    StateMachine(StateEnum startingState);

    bool iterate_once();

    StateEnum get_state();

protected:
    bool _print_repeat_transition = false;

    virtual Transition run_current_state(StateEnum currentState) = 0; // a big switch

    virtual StateEnum choose_next_state(StateEnum currentState, Transition transition) = 0; // nested switches

    virtual void print_transition(StateEnum currentState, StateEnum newState, Transition t) const = 0;

    std::string transition_to_string(Transition) const;

    void state_lock() { _state_mutex.lock(); };

    void state_unlock() { _state_mutex.unlock(); };

private:
    std::mutex _state_mutex;
    StateEnum _current_state;
};

/*---------------IMPLEMENTATION------------------*/

template<typename StateEnum>
StateMachine<StateEnum>::StateMachine(StateEnum startingState) : _current_state(startingState)
{}

template<typename StateEnum>
bool StateMachine<StateEnum>::iterate_once()
{
    std::lock_guard<std::mutex> lock(_state_mutex);
    Transition t = run_current_state(_current_state);
    if (t != T_REPEAT) {
        const StateEnum new_state = choose_next_state(_current_state, t);
        print_transition(_current_state, new_state, t);
        _current_state = new_state;
        return true;
    } else {
        if (_print_repeat_transition) {
            print_transition(_current_state, _current_state, t);
        }
        return false;
    }
}

template<typename StateEnum>
StateEnum StateMachine<StateEnum>::get_state()
{
    std::lock_guard<std::mutex> lock(_state_mutex);
    return _current_state;
}

template<typename StateEnum>
std::string StateMachine<StateEnum>::transition_to_string(Transition t) const
{
    switch (t) {
        case T_REPEAT:
            return "REPEAT";
        case T_NEXT1:
            return "NEXT1";
        case T_NEXT2:
            return "NEXT2";
        case T_NEXT3:
            return "NEXT3";
        case T_NEXT4:
            return "NEXT4";
        case T_ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}
} // namespace usm

/*---------------MACROS TO MAKE TRANSITION TABLES EASY------------------*/

// clang-format off
#define USM_TABLE(current_state, error, ...) \
switch (current_state) { \
    __VA_ARGS__; \
    default: break; \
} \
return error

#define USM_STATE(transition, start_state, ...) \
    case start_state: \
        switch (transition) { \
            __VA_ARGS__; \
            default: break; \
        } \
    break

#define USM_MAP(transition, next_state) \
            case transition: return next_state

// clang-format on
