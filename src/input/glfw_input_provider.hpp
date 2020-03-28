#pragma once

#include "input_provider.hpp"

struct GLFWwindow;

namespace input
{
	class GlfwInputProvider : public InputProvider
	{
	public:
		GlfwInputProvider(GLFWwindow* window); 

		/** Will provide all events that happened since the last call to poll events. Older events will be discarded. **/
		[[nodiscard]] virtual const std::vector<Event>& poll_events() override;

	private:
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void window_close_callback(GLFWwindow* window);

		GLFWwindow* window;
		static std::vector<Event> current_events;
	};
}