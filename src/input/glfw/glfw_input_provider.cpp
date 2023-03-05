#include <GLFW/glfw3.h>

#include <input/glfw_input_provider.hpp>
#include <stdexcept>

#include "input/event.hpp"

namespace input
{
bool GlfwInputProvider::m_user_wants_to_quit{false};
GlfwInputProvider* GlfwInputProvider::m_current_provider{nullptr};

GlfwInputProvider::GlfwInputProvider(GLFWwindow* window)
{
    if (m_current_provider) {
        throw std::runtime_error(
            "Only one instance of `GlfwInputProvider` at one time is "
            "supported.");
    }

    if (!glfwInit()) {
        throw std::runtime_error("Unable to initialize glfw");
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    m_current_provider = this;
}

GlfwInputProvider::~GlfwInputProvider()
{
    glfwTerminate();
    m_current_provider = nullptr;
}

const std::vector<input::Event>& GlfwInputProvider::poll_events() const
{
    m_events.clear();
    glfwPollEvents();
    return m_events;
};

bool GlfwInputProvider::user_wants_to_quit() noexcept
{
    return m_user_wants_to_quit;
}

namespace
{
input::Action to_input_action(int action)
{
    switch (action) {
        case GLFW_PRESS:
            [[fallthrough]];
        case GLFW_REPEAT:
            return input::Action::Press;
        case GLFW_RELEASE:
            return input::Action::Release;
    }
    throw std::runtime_error("Unhandled argument in switch statement.");
}
}  // namespace

void GlfwInputProvider::key_callback(GLFWwindow*, int key, int, int action,
                                     int mods)
{
    auto event = input::KeyEvent{.action = to_input_action(action),
                                 .key_identifier = key,
                                 .modifications = mods};

    event.x = m_current_provider->m_current_cursor_position.x;
    event.y = m_current_provider->m_current_cursor_position.y;

    m_current_provider->m_events.push_back(std::move(event));
}

void GlfwInputProvider::cursor_callback(GLFWwindow*, double xpos, double ypos)
{
    m_current_provider->m_current_cursor_position = CursorPosition{xpos, ypos};
}

void GlfwInputProvider::mouse_callback(GLFWwindow*, int button, int action,
                                       int mods)
{
    auto event = input::MouseEvent{.action = to_input_action(action),
                                   .button_identifier = button,
                                   .modifications = mods};

    event.x = m_current_provider->m_current_cursor_position.x;
    event.y = m_current_provider->m_current_cursor_position.y;

    m_current_provider->m_events.push_back(std::move(event));
}

void GlfwInputProvider::window_close_callback(GLFWwindow*)
{
    m_user_wants_to_quit = true;
}
}  // namespace input
