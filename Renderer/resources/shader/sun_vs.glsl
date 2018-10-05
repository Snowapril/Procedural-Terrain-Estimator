#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;

uniform mat4 view;
uniform mat4 project;
uniform mat4 model;

void main(void)
{
	texCoords = aTexCoords;

    vec4 position = project * view * model * vec4(aPos, 0.0, 1.0);
	gl_Position = position.xyww;
}