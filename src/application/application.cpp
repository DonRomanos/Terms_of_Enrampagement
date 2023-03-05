#include <application/application.hpp>
#include <input/event.hpp>
#include <iostream>

namespace application
{

Application::Application(std::unique_ptr<input::Provider> input_provider)
    : m_input_provider(std::move(input_provider))
{
}

namespace
{
template <typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
Overload(Ts...) -> Overload<Ts...>;

std::string_view past_tense(input::Action action)
{
    switch (action) {
        case input::Action::Press:
            return "pressed";
        case input::Action::Release:
            return "released";
    }
    throw std::runtime_error("Unhandled argument in switch statement.");
}

std::string to_string(const input::PositionalEvent& event)
{
    return std::string("(") + std::to_string(event.x) + "," +
           std::to_string(event.y) + ")";
}
}  // namespace

int Application::run()
{
    while (!m_input_provider->user_wants_to_quit()) {
        auto events = m_input_provider->poll_events();

        for (const auto& input_event : events) {
            std::visit(Overload{[](const input::KeyEvent& event) {
                                    std::cout
                                        << "Key " << event.key_identifier
                                        << " was " << past_tense(event.action)
                                        << " at Position: " << to_string(event)
                                        << "." << std::endl;
                                },
                                [](const input::MouseEvent& event) {
                                    std::cout
                                        << "Button " << event.button_identifier
                                        << " was " << past_tense(event.action)
                                        << " at Position: " << to_string(event)
                                        << "." << std::endl;
                                },
                                [](const input::CursorEvent&) {}},
                       input_event);
        }
    }
    return 0;
}

}  // namespace application
