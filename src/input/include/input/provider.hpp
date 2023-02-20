#pragma once

namespace input
{
class Provider
{
public:
    virtual ~Provider() = default;

    virtual void process_input() = 0;
    [[nodiscard]] virtual bool user_wants_to_quit() noexcept = 0;
};

};
