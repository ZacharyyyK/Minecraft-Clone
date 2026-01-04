# pragma once
# include <glad/glad.h>
# include <glm/glm.hpp>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>

class Program{
public:
    Program(std::string vertexShaderPath, std::string fragmentShaderPath);
    GLuint getID();

    void SetMat4Uniform(std::string uniform_name, glm::mat4 mat);
private:
    /*
        Source of slurp: https://stackoverflow.com/questions/116038/how-do-i-read-an-entire-file-into-a-stdstring-in-c
    */
    std::string slurp(std::ifstream& in);
    GLuint ID;
};