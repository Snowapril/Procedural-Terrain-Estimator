#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//layout (std140) uniform VP 
//{
//    mat4 view;
//    mat4 project;
//};

uniform mat4 view;
uniform mat4 project;
uniform mat4 model;

out vec2 TexCoords;

void main(void)
{
    TexCoords = aTexCoords;

    gl_Position = project * view * model * vec4(aPos, 1.0);
}