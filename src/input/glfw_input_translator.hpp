#pragma once

#include "input_translator.hpp"
#include "input_command_table.hpp"
#include "entity_commands.hpp"

struct GLFWwindow;

namespace input
{
	class GlfwInputTranslator : public InputTranslator
	{
	public:
		GlfwInputTranslator(GLFWwindow* window, const InputCommandTable& command_table);

		[[nodiscard]] virtual const std::vector<EntityCommand>& translate_input_to_commands() override;
	
	private:
		GLFWwindow* window;
		InputCommandTable command_table;
		std::vector<EntityCommand> current_commands;
	};
}