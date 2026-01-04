# pragma once
#include <glad/glad.h>

struct Vertex
{
    GLfloat x, y, z;
    GLfloat r, g, b;

    Vertex(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b);
};