#pragma once

#include <variant>

namespace input
{
    enum class Action
    {
        Press,
        Release,
    };

    // Starting from top left as (0,0).
    struct PositionalEvent
    {
        double x;
        double y;
    };

    struct KeyEvent : public PositionalEvent
    {
        Action action;
        int key_identifier;
        int modifications;
    };

    struct CursorEvent : public PositionalEvent
    {
    };

    struct MouseEvent : public PositionalEvent
    {
        Action action;
        int button_identifier;
        int modifications;
    };

    using Event = std::variant<KeyEvent, CursorEvent, MouseEvent>;
};
