#pragma once

namespace math
{
template <typename T>
struct Vector2D {
    T x;
    T y;

    constexpr Vector2D& operator+=(const Vector2D& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vector2D& operator-=(const Vector2D& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    friend constexpr Vector2D operator+(const Vector2D& lhs,
                                        const Vector2D& rhs)
    {
        return Vector2D(lhs) += rhs;
    }

    friend constexpr Vector2D operator-(const Vector2D& lhs,
                                        const Vector2D& rhs)
    {
        return Vector2D(lhs) -= rhs;
    }

    constexpr Vector2D& operator*=(const T factor)
    {
        x *= factor;
        y *= factor;
        return *this;
    }

    constexpr Vector2D operator*(const T factor)
    {
        return Vector2D(*this) *= factor;
    }

    friend constexpr bool operator<=>(const Vector2D&,
                                      const Vector2D&) = default;
};

using Vector2i = Vector2D<int>;
}  // namespace math
