# pragma once
# include <GLFW/glfw3.h>
# include <iostream>

class Window
{
public:
    Window(const char* title, int width, int height);

    void bindWindow();
    bool shouldClose();
    void swapBuffers();
    void pollEvents();

    GLFWwindow* getWindow();

private:

    int width;
    int height;

    GLFWwindow* window;
};