#include "graphics\vulkan_engine.hpp"
#include "action_factory.hpp"

int main(void)
{
    graphics::VulkanEngine renderer;

    std::unique_ptr<input::ActionFactory> action_factory{ new input::DefaultActionFactory(renderer.get_window()) };

    while (true)
    {
        auto actions = action_factory->produce_actions();
        if (std::find(std::begin(actions), std::end(actions), core::Actions::CloseApplication) != actions.end())
        {
            break;
        }
        renderer.draw_frame();
    }
}
