# pragma once
# include <glad/glad.h>
# include <iostream>
# include <vector>
#include "../vertex/vertex.h"

using std::vector;

class Triangle
{
public:
    Triangle(Vertex v1, Vertex v2, Vertex v3);

    void draw();
private:
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;

    vector<GLfloat> vertices;
    vector<GLuint> indices;
};