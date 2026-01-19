#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <print>
#include <thread>
#include <chrono>

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

float t0;
float t1;

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

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
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

    t1 = (float)glfwGetTime();
    if (lockedMouse)
        camera.ProcessCameraInput(window, t1 - t0);
    t0 = t1;
}

int main()
{

    if (!glfwInit())    
        return 1;

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    t0 = (float) glfwGetTime();
    t1 = (float) glfwGetTime();

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
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    Program p("src/shaders/vertexShader.vert", "src/shaders/fragmentShader.frag");
    
    glm::mat4 perspectiveMat = camera.getPerspective();

    p.SetMat4Uniform("perspective", perspectiveMat);

    // glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), {0.25f, 0.25f, 0.25f});
    // p.SetMat4Uniform("scale", scaleMat);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    TextureAtlas TA(2, 2);

    int cmDim = 4;
    int cmDimHalf = cmDim / 2;

    ChunkManager cm(p.getID(), cmDim);
    camera.Position.x = CHUNKSIZE_X * cmDimHalf;
    camera.Position.y = 17.0f;
    camera.Position.z = CHUNKSIZE_Z * cmDimHalf ;

    std::cout << camera.Position.x << " " << camera.Position.z << std::endl;
    std::cout << camera.Front.x << " " << camera.Front.y << " " << camera.Front.z << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // exit(1);

    glm::vec3 lastPos = camera.Position;
    glm::vec3 curPos = lastPos;

    int dx, dz;
    while(!window.shouldClose())
    {
        processInput(window.getWindow());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        p.SetMat4Uniform("view", view);

        curPos = camera.Position;
    
        cm.draw(lastPos, curPos);

        lastPos = curPos;


        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}