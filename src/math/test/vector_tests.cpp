#include <gtest/gtest.h>

#include <math/vector.hpp>

namespace math
{
// NOLINTNEXTLINE
TEST(Vector, Supports_arithmetic_operations)
{
    // Addition
    static_assert((Vector2i{.x = 1, .y = 2} + Vector2i{.x = 2, .y = 1}) ==
                  Vector2i{.x = 3, .y = 3});
    static_assert((Vector2i{.x = 1, .y = 2} += Vector2i{.x = 1, .y = 2}) ==
                  Vector2i{.x = 2, .y = 4});

    // Subtraction
    static_assert((Vector2i{.x = 1, .y = 2} -= Vector2i{.x = 2, .y = 1}) ==
                  Vector2i{.x = -1, .y = 1});
    static_assert((Vector2i{.x = 5, .y = 4} - Vector2i{.x = 1, .y = 2}) ==
                  Vector2i{.x = 4, .y = 2});

    // Multiplication
    static_assert((Vector2i{.x = 1, .y = 2} *= 3) == Vector2i{.x = 3, .y = 6});
    static_assert((Vector2i{.x = 2, .y = -1} * 3) == Vector2i{.x = 6, .y = -3});

    SUCCEED();
}
}  // namespace math
