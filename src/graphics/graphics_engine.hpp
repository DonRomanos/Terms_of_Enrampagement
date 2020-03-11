#pragma once

struct GLFWwindow;

namespace graphics
{
	class Engine
	{
	public:
		virtual void draw_frame() = 0;

		[[nodiscard]] virtual GLFWwindow* get_window() noexcept = 0;

		virtual ~Engine() {};
	};
}