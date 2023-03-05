#pragma once

#include <input/event.hpp>
#include <vector>

namespace input
{
class Provider
{
public:
    virtual ~Provider() = default;

    [[nodiscard]] virtual const std::vector<input::Event>& poll_events() const = 0;
    [[nodiscard]] virtual bool user_wants_to_quit() noexcept = 0;
};

};
