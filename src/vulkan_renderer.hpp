#pragma once

#include "renderer.hpp"
#include "vulkan_setup_helper.hpp"

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
		virtual void draw_frame() override;

		~VulkanRenderer() override;
	private:
		//void create_surface();
		void create_swapchain();
		void create_imageviews();
		void create_graphics_pipeline();
		void create_render_pass();
		void create_framebuffers();
		void create_command_pool();
		void create_command_buffers();
		void create_semaphores();

		VulkanSetupHelper setup_helper;

		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphics_queue = VK_NULL_HANDLE;
		VkSurfaceKHR rendering_surface = VK_NULL_HANDLE;
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> swapchain_images = {};
		std::vector<VkImageView> swapchain_imageviews = {};
		std::vector<VkFramebuffer> framebuffers = {};
		std::vector<VkCommandBuffer> command_buffers = {};
		VkFormat swapchain_format = VkFormat::VK_FORMAT_UNDEFINED;
		VkExtent2D swapchain_extent = VkExtent2D{};
		VkRenderPass render_pass = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkCommandPool command_pool = VK_NULL_HANDLE;
		VkSemaphore image_available;
		VkSemaphore rendering_finished;
		GLFWwindow* window = nullptr;
	};
}