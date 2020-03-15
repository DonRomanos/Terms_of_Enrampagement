#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vector>

struct GLFWwindow;

namespace graphics
{
	struct QueueFamily
	{
		uint32_t index;
		VkQueueFamilyProperties properties;
	};
	bool operator==(const QueueFamily& lhs, const QueueFamily& rhs) noexcept;


	class VulkanSetupHelper
	{
	public:
		explicit VulkanSetupHelper(GLFWwindow* window);
		~VulkanSetupHelper() noexcept;

		VulkanSetupHelper(const VulkanSetupHelper& other) = delete;
		VulkanSetupHelper& operator=(const VulkanSetupHelper& other) = delete;

		VulkanSetupHelper(VulkanSetupHelper&& other) = default;
		VulkanSetupHelper& operator=(VulkanSetupHelper&& other) = default;

		[[nodiscard]] VkDevice acquire_device(
			const std::vector<const char*>& required_extensions, 
			const std::vector<const char*>& required_validation_layers,
			const std::vector<const char*>& device_extensions
		);

		// Todo check what of and when this can be removed. --> after refactoring of engine
		[[nodiscard]] VkInstance get_instance() const noexcept { return instance; }
		[[nodiscard]] VkPhysicalDevice get_physical_device() const noexcept { return physical_device; }
		[[nodiscard]] VkSurfaceKHR get_surface() const noexcept { return surface; }
		[[nodiscard]] VkQueue get_graphics_queue() const noexcept { return graphics_queue; }
		[[nodiscard]] VkQueue get_present_queue() const noexcept { return present_queue; }
	private:
		GLFWwindow* window;

		// TODO add RAI wrapper for destruction support.
		VkInstance instance;
		VkPhysicalDevice physical_device;
		VkSurfaceKHR surface;
		VkDevice device;
		VkQueue graphics_queue;
		VkQueue present_queue;
	};
}