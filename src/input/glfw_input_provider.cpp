#include "glfw_input_provider.hpp"

using namespace input;

#include "GLFW/glfw3.h"

input::GlfwInputProvider::GlfwInputProvider(GLFWwindow* window) : window(window)
{
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowCloseCallback(window, window_close_callback);
}

void input::GlfwInputProvider::key_callback(GLFWwindow*, int key, int, int action, int mods)
{
}

void input::GlfwInputProvider::window_close_callback(GLFWwindow*)
{
}
