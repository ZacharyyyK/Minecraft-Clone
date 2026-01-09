# version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 perspective;
uniform mat4 view;
uniform vec3 cc;

void main()
{
    gl_Position =  perspective * view * vec4(aPos + cc, 1.0);
    texCoord = aTexCoord;
}