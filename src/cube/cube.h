# pragma once

# include <glad/glad.h>
# include <iostream>
# include <vector>
# include <array>
# include "../stb_image/stb_image.h"

class Cube
{
public:
    Cube();
    virtual ~Cube();
    void draw();
protected:

    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    GLuint TEXTURE;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};