#pragma once

#include <algorithm>
#include <vector>

namespace utility
{
template <typename T>
class OrderIndependentVector
{
    using difference_type = typename std::vector<T>::difference_type;

   public:
    struct ReserveSpace {
        constexpr explicit ReserveSpace(size_t size) : size(size) {}
        size_t size;
    };
    struct Index {
        constexpr explicit Index(difference_type index) : index(index) {}
        difference_type index;
    };

    constexpr OrderIndependentVector() = default;
    constexpr OrderIndependentVector(ReserveSpace size_to_reserve) : m_storage()
    {
        m_storage.reserve(size_to_reserve.size);
    }

    [[nodiscard]] constexpr size_t size() const noexcept
    {
        return m_storage.size();
    }

    [[nodiscard]] constexpr T& operator[](size_t index)
    {
        return m_storage[index];
    }
    [[nodiscard]] constexpr const T& operator[](size_t index) const
    {
        return m_storage[index];
    }

    template <typename... Args>
    constexpr T& emplace(Args&&... args)
    {
        m_storage.emplace_back(std::forward<Args>(args)...);
        return m_storage.back();
    }

    constexpr void erase(Index index)
    {
        std::iter_swap(m_storage.end(), m_storage.begin() + index.index);
        m_storage.pop_back();
    }

   private:
    std::vector<T> m_storage{};
};
}  // namespace utility
