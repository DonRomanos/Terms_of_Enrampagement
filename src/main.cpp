#include "graphics\vulkan_engine.hpp"

int main(void)
{
    graphics::VulkanEngine renderer;

    while (true)
    {
        renderer.draw_frame();
    }
    //while (!glfwWindowShouldClose(window))
    //{
    //    glfwPollEvents();
    //    renderer.draw_frame();
    //}
}
