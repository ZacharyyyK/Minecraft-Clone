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

float t0 ;
float t1 ;

float x0 = (float) (WIDTH / 2);
float y02 = (float) (HEIGHT / 2);

bool firstMouse = true;
bool lockedMouse = true;

bool renderWireframe = false;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    if (firstMouse)
    {
        x0 = (float) xpos;
        y02 = (float) ypos;
        firstMouse = false;
    }

    float dx = ((float) xpos) - x0;
    float dy = ((float) ypos) - y02;

    camera.ProcessMouseMovement(dx, dy);

    x0 = xpos;
    y02 = ypos;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) 
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

    if (key == GLFW_KEY_P and action == GLFW_PRESS)
    {
        renderWireframe = !renderWireframe;

        if (renderWireframe) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    // if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    // {
    //     lockedMouse = !lockedMouse;

    //     if (lockedMouse)
    //     {
    //         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //         glfwSetCursorPosCallback(window, mouse_callback);
    //         firstMouse = true;
    //     }
    //     else
    //     {
    //         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //         glfwSetCursorPosCallback(window, nullptr);
    //     }
    // }

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Specify major version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Specify minor version 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile

    // The forward compatibility hint is recommended, especially for macOS
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfwInit();

    t0 = (float) glfwGetTime();
    t1 = (float) glfwGetTime();

    const char* descrip = nullptr;
    int code = glfwGetError(&descrip);
    if (descrip) std::cout << descrip << std::endl;
    
    if (descrip) return -1;

    GLFWmonitor* MyMonitor =  glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(MyMonitor);

    WIDTH = 1200;
    HEIGHT = 800;

    camera.setWidth(WIDTH);
    camera.setHeight(HEIGHT);

    Window window("Minecraft", WIDTH, HEIGHT);

    glfwSetCursorPosCallback(window.getWindow(), mouse_callback);
    glfwSetKeyCallback(window.getWindow(), key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
        return 1;

    glEnable(GL_DEPTH_TEST);

    Program p("src/shaders/vertexShader.vert", "src/shaders/fragmentShader.frag");
    
    glm::mat4 perspectiveMat = camera.getPerspective();
    p.SetMat4Uniform("perspective", perspectiveMat);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    TextureAtlas TA(2, 2);
    
    
    // Chunk chunk;
    // array<pair<float, float>, 4> sideGrassCoords = TA.getCoordsForBlock(0,0);
    auto sideGrassCoords = TA.getCoordsForBlock("Side Grass");
    auto botGrassCords = TA.getCoordsForBlock(0, 1);
    auto topGrassCords = TA.getCoordsForBlock(1,0);

    // GrassBlock cube(sideGrassCoords, botGrassCords, topGrassCords);

    for (int i = 0; i < 4; ++i)
    {
        std::cout << botGrassCords[i].first << " " << botGrassCords[i].second << std::endl;
    }
    // exit(1);

    // Chunk chunk;

    ChunkManager cm(p.getID());

    while(!window.shouldClose())
    {
        processInput(window.getWindow());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        p.SetMat4Uniform("view", view);

        // tri.draw();
        // cube.draw();
        // chunk.draw();
        cm.draw();

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
