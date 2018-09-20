#version 430 core

layout (location = 0) in vec2 aPos;

out vec2 texCoords;

layout (std140) VP {
    mat4 view;
    mat4 project;
}

uniform mat4 model;

void main(void) 
{
    texCoords = vec2(aPos.x / 2.0 + 0.5, aPos.y / 2.0 + 0.5);
    gl_Position = project * view * model * vec4(aPos.x, 0.0, aPos.y, 1.0);
}