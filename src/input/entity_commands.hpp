#pragma once

namespace input
{
	enum class CommandId
	{
		WindowClosed,
		Move,
	};

	struct Point
	{
		int x;
		int y;
	};

	struct EntityCommand
	{
		CommandId id;
		int source_entity;
		int target_entity;
		Point target_coords;
		Point source_coords;
	};
}