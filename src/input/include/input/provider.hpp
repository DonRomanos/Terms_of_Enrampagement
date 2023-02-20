#pragma once

namespace input
{
class Provider
{
public:
    virtual ~Provider() = default;
    virtual void process_input() = 0;
};

};
