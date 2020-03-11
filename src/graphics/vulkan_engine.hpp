#pragma once

#include "graphics_engine.hpp"

#include <memory>

namespace graphics
{
	class VulkanRenderer;

	class VulkanEngine : public Engine
	{
	public:
		VulkanEngine();
		virtual ~VulkanEngine() override;

		virtual void draw_frame() override;

		[[nodiscard]] virtual GLFWwindow* get_window() noexcept override;
	private:
		std::unique_ptr<graphics::VulkanRenderer> impl;
	};
}