#pragma once

namespace graphics
{
	class Renderer
	{
	public:
		virtual void init() = 0;
		virtual void render() = 0;

		virtual ~Renderer() {};
	};
}