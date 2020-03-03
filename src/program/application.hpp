#pragma once

#include "dispatcher.hpp"
#include "renderer.hpp"

#include <memory>

class Application
{
public:
	Application(std::unique_ptr<input::Dispatcher>&& dispatcher, std::unique_ptr<graphics::Renderer>&& renderer);

	void init();
	void execute_main_loop();

private:
	std::unique_ptr<input::Dispatcher> dispatcher;
	std::unique_ptr<graphics::Renderer> renderer;
};