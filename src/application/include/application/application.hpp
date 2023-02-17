#pragma once

#include <input/provider.hpp>

#include <memory>

namespace application
{

class Application
{
public:
	Application(std::unique_ptr<input::Provider> input_provider);

	int run();

private:
	std::unique_ptr<input::Provider> m_input_provider;
};

};
