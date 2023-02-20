#include "application/application.hpp"

namespace application
{

Application::Application(std::unique_ptr<input::Provider> input_provider) 
	: m_input_provider(std::move(input_provider))
{
}

int Application::run()
{
	while(!m_input_provider->user_wants_to_quit())
	{
		m_input_provider->process_input();
	}
	return 0;
}

}
