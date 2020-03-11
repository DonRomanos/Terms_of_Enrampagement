#pragma once

#include "entity_commands.hpp"

#include <map>

namespace input
{
	using KeyId = int;
	using InputCommandTable = std::map<KeyId, EntityCommand>;

	const InputCommandTable default_command_table;
}