#version 430 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 project;

out vec3 texCoords;

void main(void)
{
    texCoords = aPos;
    vec4 position = project * view * vec4(aPos, 1.0);
    gl_Position = position.xyww;
}