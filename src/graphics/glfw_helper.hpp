#pragma once

struct GLFWwindow;

namespace graphics
{
	class GlfwHelper
	{
	public:
		GlfwHelper(int window_widht, int window_height);
		~GlfwHelper();

		[[nodiscard]] GLFWwindow* get_window() const noexcept;

	private:
		GLFWwindow* window;
	};
}