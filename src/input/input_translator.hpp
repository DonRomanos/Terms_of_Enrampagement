#pragma once

#include "entity_commands.hpp"

#include <vector>

namespace input
{
	class InputTranslator
	{
	public:
		[[nodiscard]] virtual const std::vector<EntityCommand>& translate_input_to_commands() = 0;
	};
}