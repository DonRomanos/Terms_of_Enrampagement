#pragma once

#include <input/event.hpp>
#include <span>

namespace input
{
class Provider
{
   public:
    virtual ~Provider() = default;

    [[nodiscard]] virtual const std::span<input::Event> poll_events() const = 0;
    [[nodiscard]] virtual bool user_wants_to_quit() noexcept = 0;
};

};  // namespace input
