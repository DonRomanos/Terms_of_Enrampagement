#include "glfw_helper.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

using namespace graphics;

GlfwHelper::GlfwHelper(int window_width, int window_height)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(window_width, window_height, "Terms of Enrampagement", nullptr, nullptr);
}

GlfwHelper::~GlfwHelper()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* GlfwHelper::get_window() const noexcept
{
    return window;
}