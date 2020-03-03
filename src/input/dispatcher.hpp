#pragma once

namespace input
{
	enum class GameStatus
	{
		Running,
		Terminated
	};

	class Dispatcher
	{
	public:
		virtual GameStatus handleEvents() = 0;
	};
}