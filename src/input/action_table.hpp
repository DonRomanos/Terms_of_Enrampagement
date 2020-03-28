#pragma once

#include "input_provider.hpp"
#include "actions.hpp"

#include <map>

namespace input
{
	using InputActionTable = std::map<input::Event, core::Actions>;

	const InputActionTable default_action_table{ {WindowShouldClose, core::Actions::CloseApplication } };
}