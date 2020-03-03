#include "graphics\vulkan_renderer.hpp"

int main(void)
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    // Important since renderer refers window
    {
        graphics::VulkanRenderer renderer(window);
        renderer.init();

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            renderer.draw_frame();
        }
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
}
