# include "window.h"

Window::Window(const char* title, int width, int height) : width(width), height(height)
{


    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window)
    {
        std::cout << "Failed to create window..." << std::endl;
        return;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);

}

void Window::bindWindow() { glfwMakeContextCurrent(window); }
bool Window::shouldClose() { return glfwWindowShouldClose(window); }
void Window::swapBuffers() { glfwSwapBuffers(window); }
void Window::pollEvents() { glfwPollEvents(); }
GLFWwindow* Window::getWindow() { return window; }
