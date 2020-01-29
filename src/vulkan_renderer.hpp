#pragma once

#include "renderer.hpp"

namespace graphics
{
	class VulkanRenderer : public Renderer
	{
		virtual void init() override;
		virtual void render() override;
	};
}