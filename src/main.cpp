#include <GLFW/glfw3.h>

#include <application/application.hpp>
#include <input/glfw_input_provider.hpp>
#include <iostream>

#include "application/include/application/application.hpp"

int main()
{
    auto window = [] {
        if (auto init_result = glfwInit(); !init_result) {
            std::cerr << "Initialization of GLFW failed with code: "
                      << init_result << ", terminating." << std::endl;
            std::terminate();
        }

        //   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        //   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        //   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        return glfwCreateWindow(640, 480, "Terms of Enrampagement", NULL, NULL);
    }();

    if (!window) {
        std::cerr << "Unable to create glfw window, terminating." << std::endl;
        return 1000;
    }

    auto glfw_input_provider =
        std::make_unique<input::GlfwInputProvider>(window);
    application::Application app(std::move(glfw_input_provider));

    return app.run();
}
