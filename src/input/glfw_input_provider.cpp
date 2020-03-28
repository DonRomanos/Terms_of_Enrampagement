#include "glfw_input_provider.hpp"

using namespace input;

#include "GLFW/glfw3.h"

input::GlfwInputProvider::GlfwInputProvider(GLFWwindow* window) : window(window)
{
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowCloseCallback(window, window_close_callback);
}

const std::vector<Event>& input::GlfwInputProvider::poll_events()
{
	current_events.clear();

	glfwPollEvents();

	return current_events;
}

void input::GlfwInputProvider::key_callback(GLFWwindow*, int key, int, int action, int mods)
{
	current_events.push_back(Event{ key, action, mods });
}

void input::GlfwInputProvider::window_close_callback(GLFWwindow*)
{
	current_events.push_back(WindowShouldClose);
}

std::vector<input::Event> input::GlfwInputProvider::current_events{ 200 };