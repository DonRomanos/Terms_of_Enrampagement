#pragma once

#include <math/vector.hpp>

namespace math
{
template <typename T>
struct Rect {
    Vector2D<T> top_left;
    T width;
    T height;

    friend constexpr bool operator<=>(const Rect&, const Rect&) = default;
};

using Rect2i = Rect<int>;
}  // namespace math
