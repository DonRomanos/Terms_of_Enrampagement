#include <gtest/gtest.h>

#include <utility/order_independent_vector.hpp>

namespace utility
{
// NOLINTNEXTLINE
TEST(OrderIndependentVector, Deleting_of_single_objects)
{
    OrderIndependentVector<int> uut;

    uut.emplace(1);
    uut.emplace(11);
    uut.emplace(112);

    EXPECT_EQ(uut.size(), 3);
    EXPECT_EQ(uut[1], 11);

    uut.erase(OrderIndependentVector<int>::Index{1});

    EXPECT_EQ(uut[1], 112);
}
}  // namespace utility
