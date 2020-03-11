#include "glfw_input_translator.hpp"

using namespace input;

input::GlfwInputTranslator::GlfwInputTranslator(GLFWwindow* window, const InputCommandTable& command_table) : window(window), command_table(command_table), current_commands()
{
	current_commands.reserve(100);
}

const std::vector<EntityCommand>& input::GlfwInputTranslator::translate_input_to_commands()
{
	return current_commands;
}

