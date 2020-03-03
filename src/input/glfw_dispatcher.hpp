#pragma once

#include "dispatcher.hpp"

namespace input
{
	class GlfwWindowDispatcher : public Dispatcher
	{
	public:
		virtual GameStatus handleEvents() override;
	};
}