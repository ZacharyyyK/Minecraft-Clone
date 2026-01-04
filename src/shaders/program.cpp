# include "program.h"

std::string Program::slurp(std::ifstream& in)
{
    // https://stackoverflow.com/questions/116038/how-do-i-read-an-entire-file-into-a-stdstring-in-c
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}

Program::Program(std::string vertexShaderPath, std::string fragmentShaderPath){
    

    std::ifstream vertexShaderStream(vertexShaderPath);

    if (vertexShaderStream.fail())
    {
        std::cout << "Failed to create vertexShaderStream... (shaders/program.cpp [line 14])";
        exit(1);
    }

    std::ifstream fragShaderStream(fragmentShaderPath);

    if (fragShaderStream.fail())
    {
        std::cout << "Failed to create fragShaderStream... (shaders/program.cpp [line 22])";
        exit(1);
    }

    const std::string vertexShader = slurp(vertexShaderStream);
    const std::string fragmentShader = slurp(fragShaderStream);

    const char* vShaderCode = vertexShader.c_str();
    const char* fShaderCode = fragmentShader.c_str();
    
    GLuint vertex, fragment;
    int success;
    char infoLog[1024];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    glUseProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

GLuint Program::getID(){ return ID; }

void Program::SetMat4Uniform(std::string uniform_location, glm::mat4 mat)
{
    GLint loco = glGetUniformLocation(ID, uniform_location.c_str());
    if (loco < 0)
    {
        std::cout << "Failed to find location of uniform: " << uniform_location << std::endl;
        exit(1);
    }
    
    glUniformMatrix4fv(loco, 1, GL_FALSE, &mat[0][0]);
}