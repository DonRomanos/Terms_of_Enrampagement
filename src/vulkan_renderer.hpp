#pragma once

#include "renderer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace graphics
{
	class VulkanRenderer : public Renderer
	{
	public:
		virtual void init() override;
		virtual void render() override;

		~VulkanRenderer() override;
	private:
		void create_instance();
		void pick_physical_device();
		void create_logical_device();

		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue queue = VK_NULL_HANDLE;
	};
}