#include "gtest/gtest.h"

#include "action_factory.hpp"
#include "input_provider.hpp"

#include <utility>

namespace
{
    class SampleInputProvider : public input::InputProvider
    {
    public: 
        SampleInputProvider(std::initializer_list<input::Event> events) : events{ events } {};

        virtual const std::vector<input::Event>& poll_events() override { return events; };

    private:
        std::vector<input::Event> events;
    };
}

TEST(DefaultActionFactoryTests, Event_not_mapped_to_an_action_should_produce_no_actions)
{
    auto provider = std::unique_ptr<input::InputProvider>(new SampleInputProvider{ input::Event{5,0,7} });
    input::DefaultActionFactory to_test{ std::move(provider), input::default_action_table };

    auto result = to_test.produce_actions();

    EXPECT_TRUE(result.empty());
}

TEST(DefaultActionFactoryTests, WindowShouldClose_event_produces_CloseApplication_action)
{
    auto provider = std::unique_ptr<input::InputProvider>(new SampleInputProvider{ input::WindowShouldClose });
    input::DefaultActionFactory to_test{ std::move(provider), input::default_action_table };
    std::vector<core::Actions> expected = {core::Actions::CloseApplication};

    auto result = to_test.produce_actions();

    EXPECT_EQ(result, expected);
}

// Had some Problems with the conan cmake_find_package generator, 
// it does not generate the correct targets therefore we dont have a gtest_main target
// so I write my own main.
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
