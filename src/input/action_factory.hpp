#pragma once

#include "actions.hpp"
#include "input_provider.hpp"
#include "action_table.hpp"

#include <vector>
#include <memory>

struct GLFWwindow;

namespace input
{
	class ActionFactory
	{
	public:
		[[nodiscard]] virtual std::vector<core::Actions> produce_actions() = 0;
		virtual ~ActionFactory() = default;
	};

	class DefaultActionFactory : public ActionFactory
	{
	public:
		explicit DefaultActionFactory(GLFWwindow* window);
		DefaultActionFactory(std::unique_ptr<InputProvider>&& input_source, InputActionTable action_table);

		[[nodiscard]] virtual std::vector<core::Actions> produce_actions();

	private:
		std::unique_ptr<InputProvider> input_source;
		InputActionTable action_table;
	};
}