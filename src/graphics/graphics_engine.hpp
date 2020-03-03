#pragma once

namespace graphics
{
	class Renderer
	{
	public:
		virtual void init() = 0;
		virtual void draw_frame() = 0;

		virtual ~Renderer() {};
	};
}