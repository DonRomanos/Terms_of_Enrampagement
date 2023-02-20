#include <input/glfw_input_provider.hpp>

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

namespace input
{
bool GlfwInputProvider::m_user_wants_to_quit{false};

GlfwInputProvider::GlfwInputProvider(GLFWwindow* window) : window(window)
{
	if(!glfwInit())
	{
		throw std::runtime_error("Unable to initialize glfw");
	}
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowCloseCallback(window, window_close_callback);
}

GlfwInputProvider::~GlfwInputProvider()
{
	glfwTerminate();
}

void GlfwInputProvider::process_input()
{
	glfwPollEvents();
}

bool GlfwInputProvider:: user_wants_to_quit() noexcept
{
	return m_user_wants_to_quit;
}

void GlfwInputProvider::key_callback(GLFWwindow*, int key, int, int action, int mods)
{
	(void) key;
	(void) action;
	(void) mods;
	std::cout << "Key event" << std::endl;
}

void GlfwInputProvider::window_close_callback(GLFWwindow*)
{
	std::cout << "window closed" << std::endl;
	m_user_wants_to_quit = true;
}
}
