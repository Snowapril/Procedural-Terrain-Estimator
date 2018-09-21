#version 430 core

layout (location = 0) in vec2 aPos;

out vec4 clipPosition;

layout (std140) uniform VP 
{
    mat4 view;
    mat4 project;
};

uniform mat4 model;

void main(void) 
{
	clipPosition = project * view * model * vec4(aPos.x, 0.0, aPos.y, 1.0);
	gl_Position = clipPosition;
}