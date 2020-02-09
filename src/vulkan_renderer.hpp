#pragma once

#include "renderer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace graphics
{
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer(GLFWwindow* window);

		virtual void init() override;
		virtual void render() override;

		~VulkanRenderer() override;
	private:
		void create_instance();
		void pick_physical_device();
		void create_logical_device();
		void create_surface();
		void create_swapchain();

		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphics_queue = VK_NULL_HANDLE;
		VkSurfaceKHR rendering_surface = VK_NULL_HANDLE;
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> swapchain_images;
		VkFormat swapchain_format;
		VkExtent2D swapchain_extent;
		GLFWwindow* window = nullptr;
	};
}