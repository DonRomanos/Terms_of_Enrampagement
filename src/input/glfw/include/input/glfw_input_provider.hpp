#pragma once

#include <input/event.hpp>
#include <input/provider.hpp>
#include <unordered_map>
#include <vector>

struct GLFWwindow;

namespace input
{
class GlfwInputProvider : public input::Provider
{
   public:
    GlfwInputProvider(GLFWwindow* window);
    GlfwInputProvider(const GlfwInputProvider& other) = delete;
    GlfwInputProvider operator=(const GlfwInputProvider& other) = delete;
    GlfwInputProvider(GlfwInputProvider&& other) = delete;
    GlfwInputProvider operator=(GlfwInputProvider&& other) = delete;
    ~GlfwInputProvider() override;

    [[nodiscard]] const std::vector<input::Event>& poll_events() const override;
    [[nodiscard]] bool user_wants_to_quit() noexcept override;

   private:
    static void key_callback(GLFWwindow* window, int key, int scancode,
                             int action, int mods);
    static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_callback(GLFWwindow* window, int button, int action,
                               int mods);
    static void window_close_callback(GLFWwindow* window);

    static bool m_user_wants_to_quit;
    static GlfwInputProvider* m_current_provider;

    struct CursorPosition {
        double x{-1.0};
        double y{-1.0};
    };

    CursorPosition m_current_cursor_position{};
    GLFWwindow* m_window{nullptr};
    mutable std::vector<input::Event> m_events{};
};
}  // namespace input
