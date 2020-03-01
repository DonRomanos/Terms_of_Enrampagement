#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace graphics
{
	const std::vector<const char*> default_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	struct QueueFamily
	{
		uint32_t index;
		VkQueueFamilyProperties properties;
	};
	bool operator==(const QueueFamily& lhs, const QueueFamily& rhs) noexcept;


	class VulkanSetupHelper
	{
	public:
		VulkanSetupHelper(GLFWwindow* window);
		~VulkanSetupHelper() noexcept;

		VulkanSetupHelper(const VulkanSetupHelper& other) = delete;
		VulkanSetupHelper& operator=(const VulkanSetupHelper& other) = delete;

		//TODO add move support
		[[nodiscard]] VkDevice acquire_device(
			const std::vector<const char*>& required_extensions, 
			const std::vector<const char*>& required_validation_layers,
			const std::vector<const char*>& device_extensions = default_device_extensions
		);

		// Todo check what of and when this can be removed. --> after refactoring
		[[nodiscard]] VkInstance get_instance() const noexcept { return instance; }
		[[nodiscard]] VkPhysicalDevice get_physical_device() const noexcept { return physical_device; }
		[[nodiscard]] VkSurfaceKHR get_surface() const noexcept { return surface; }
		[[nodiscard]] VkQueue get_graphics_queue() const noexcept { return graphics_queue; }
		[[nodiscard]] VkQueue get_present_queue() const noexcept { return present_queue; }
	private:
		GLFWwindow* window;
		VkInstance instance;
		VkPhysicalDevice physical_device;
		VkSurfaceKHR surface;
		VkDevice device;
		VkQueue graphics_queue;
		VkQueue present_queue;
	};
}