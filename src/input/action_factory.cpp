#include "action_factory.hpp"
#include "glfw_input_provider.hpp"

using namespace input;

input::DefaultActionFactory::DefaultActionFactory(GLFWwindow* window)
	: input_source(std::unique_ptr<InputProvider>(new GlfwInputProvider(window)))
	, action_table(default_action_table)
{
}

input::DefaultActionFactory::DefaultActionFactory(std::unique_ptr<InputProvider>&& input_source, InputActionTable action_table)
	: input_source(std::move(input_source))
	, action_table(action_table)
{
}

std::vector<core::Actions> input::DefaultActionFactory::produce_actions()
{
	std::vector<core::Actions> result;

	for (const auto& event : input_source->poll_events())
	{
		auto corresponding_action = action_table.find(event);
		if (corresponding_action != action_table.end())
		{
			result.emplace_back(corresponding_action->second);
		}
	}

	return result;
}
