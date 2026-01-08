# pragma once

# include <glad/glad.h>
# include <iostream>
# include <vector>
# include <array>
# include "../stb_image/stb_image.h"

class Block
{
public:
    Block();
    virtual ~Block();
    void draw();
protected:

    void setFaceUVs(int faceIndex, const std::array<std::pair<float, float>, 4>& uvs);
    void SendVertexData();
    

    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    GLuint TEXTURE;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};