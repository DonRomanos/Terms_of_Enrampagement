#pragma once

#include <input/provider.hpp>

struct GLFWwindow;

namespace input
{
	class GlfwInputProvider : public input::Provider
	{
	public:
		GlfwInputProvider(GLFWwindow* window);
		~GlfwInputProvider(); 

		void process_input();

	private:
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void window_close_callback(GLFWwindow* window);

		GLFWwindow* window;
	};
}
