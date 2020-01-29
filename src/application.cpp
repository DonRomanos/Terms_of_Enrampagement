#include "application.hpp"

Application::Application(std::unique_ptr<input::Dispatcher>&& dispatcher, std::unique_ptr<graphics::Renderer>&& renderer) 
	: dispatcher(std::move(dispatcher))
	, renderer(std::move(renderer))
{
}

void Application::init()
{
	renderer->init();
}

void Application::execute_main_loop()
{
	while (dispatcher->handleEvents() != input::GameStatus::Terminated)
	{
		renderer->render();
	}
}
