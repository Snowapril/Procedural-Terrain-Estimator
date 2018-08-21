#version 430 core

layout(vertices = 3) out;

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