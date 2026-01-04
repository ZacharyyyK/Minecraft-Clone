# include "window.h"

Window::Window(const char* title, int width, int height) : width(width), height(height)
{

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window)
    {
        println("Failed to create window...");
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