/**
 * @file
 * @copyright (C) 2022-2023 BMW AG
 */

#pragma once

namespace fsm {

/**
 * @brief Base class that every declared event must derive from.
 */
struct Event {
    Event() = default;
    Event(const Event&) = default;
    Event& operator=(const Event&) = default;
    Event(Event&&) noexcept = default;
    Event& operator=(Event&&) noexcept = default;
    virtual ~Event() = default;
};

}  // namespace fsm
