#include "application/application.hpp"

namespace application
{

Application::Application(std::unique_ptr<input::Provider> input_provider) 
	: m_input_provider(std::move(input_provider))
{
}

int Application::run()
{
	while(true);
}

}
