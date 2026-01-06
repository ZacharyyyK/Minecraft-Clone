#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <print>

#include "stb_image/stb_image.h"
#include "window/window.h"
#include "vertex/vertex.h"
#include "triangle/triangle.h"
#include "shaders/program.h"
#include "camera/camera.h"
#include "block/block.h"
#include "grassblock/grassBlock.h"
#include "textureAtlas/textureAtlas.h"
#include "voxelBlocks/voxelChunk.h"

int WIDTH = 1200;
int HEIGHT = 800;

// enum Camera_Movement {
//     FORWARD,
//     BACKWARD,
//     LEFT,
//     RIGHT
// };

Camera camera;

float t0 = (float) glfwGetTime();
float t1 = (float) glfwGetTime();

float x0 = (float) (WIDTH / 2);
float y0 = (float) (HEIGHT / 2);

bool firstMouse = true;
bool lockedMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    if (firstMouse)
    {
        x0 = (float) xpos;
        y0 = (float) ypos;
        firstMouse = false;
    }

    float dx = ((float) xpos) - x0;
    float dy = ((float) ypos) - y0;

    camera.ProcessMouseMovement(dx, dy);

    x0 = xpos;
    y0 = ypos;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) // edge-trigger: only once per press
    {
        lockedMouse = !lockedMouse;

        if (lockedMouse)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPosCallback(window, mouse_callback);
            firstMouse = true;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursorPosCallback(window, nullptr);
        }
    }

    // Only move camera when locked, if you want "pause"
    t1 = (float)glfwGetTime();
    if (lockedMouse)
        camera.ProcessCameraInput(window, t1 - t0);
    t0 = t1;
}


int main()
{

    if (!glfwInit())    
        return 1;

    GLFWmonitor* MyMonitor =  glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(MyMonitor);

    WIDTH = 1200;
    HEIGHT = 800;

    camera.setWidth(WIDTH);
    camera.setHeight(HEIGHT);

    Window window("Minecraft", WIDTH, HEIGHT);
    glfwSetCursorPosCallback(window.getWindow(), mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
        return 1;

    glEnable(GL_DEPTH_TEST);

    Program p("src/shaders/vertexShader.vert", "src/shaders/fragmentShader.frag");
    // TextureAtlas("src/textures/TextureAtlas.png", 4, 4);

    // Triangle tri(Vertex(-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f), Vertex(0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f), Vertex(0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f));

    glm::mat4 perspectiveMat = camera.getPerspective();
    p.SetMat4Uniform("perspective", perspectiveMat);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    TextureAtlas TA(2, 2);
    // std::vector<std::pair<float, float>> sideGrassCoords = TA.getCoordsForBlock(0,0);
    // auto sideGrassCoords = TA.getCoordsForBlock("Side Grass");
    // auto botGrassCords = TA.getCoordsForBlock(0, 1);
    // auto topGrassCords = TA.getCoordsForBlock(1,0);

    // GrassBlock cube(sideGrassCoords, botGrassCords, topGrassCords);

    Chunk chunk;

    while(!window.shouldClose())
    {
        processInput(window.getWindow());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        p.SetMat4Uniform("view", view);

        // tri.draw();
        // cube.draw();
        chunk.draw();

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}