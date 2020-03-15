#include "vulkan_engine.hpp"
#include "vulkan_renderer.hpp"

using namespace graphics;

graphics::VulkanEngine::VulkanEngine() : impl(new VulkanRenderer())
{
}

graphics::VulkanEngine::~VulkanEngine() = default;

void graphics::VulkanEngine::draw_frame()
{
	impl->draw_frame();
}

GLFWwindow* graphics::VulkanEngine::get_window() noexcept
{
	return impl->get_window();
}
