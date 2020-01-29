#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main(void)
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);

    glfwTerminate();
}
